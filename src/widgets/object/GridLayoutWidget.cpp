#include "GridLayoutWidget.h"

#include "widgets/jsonKeys.h"

#include <QCheckBox>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

namespace object
{

	namespace
	{
		constexpr auto defaultPropertyOrder = 1000;

		//----------------------------------------------------------------------
		auto getPropertyKeyComparer(QJsonObject properties)
		{
			return [=](const QString& lhs, const QString& rhs) {
				const auto proj = [&](QString key) {
					return std::tuple(
						properties[key][json_keys::key_propertyOrder].toInt(
							defaultPropertyOrder),
						key);
				};
				return proj(lhs) < proj(rhs);
			};
		}

		//----------------------------------------------------------------------
		std::vector<QString> toStrings(QJsonArray array)
		{
			std::vector<QString> res;
			for (const auto& value : array)
			{
				res.push_back(value.toString());
			}
			return res;
		}

		//----------------------------------------------------------------------
		void insertRow(QGridLayout& grid_layout, int rowCount)
		{
			for (int row = grid_layout.rowCount() - 1; row >= rowCount; --row)
			{
				for (int col = 0; col != grid_layout.columnCount(); ++col)
				{
					auto item = grid_layout.itemAtPosition(row, col);
					if (item)
					{
						grid_layout.removeItem(item);
						grid_layout.addItem(item, row + 1, col);
					}
				}
			}
		}

		//----------------------------------------------------------------------
		void deleteRow(QGridLayout& grid_layout, int rowCount)
		{
			for (int row = rowCount; row < grid_layout.rowCount() - 1; ++row)
			{
				for (int col = 0; col != grid_layout.columnCount(); ++col)
				{
					auto item = grid_layout.itemAtPosition(row + 1, col);
					if (item)
					{
						grid_layout.removeItem(item);
						grid_layout.addItem(item, row, col);
					}
				}
			}
		}

		//----------------------------------------------------------------------
		auto byName(QString name)
		{
			return [=](const auto& p) { return p->name == name; };
		}

		//----------------------------------------------------------------------
		template <typename T>
		bool contains(const std::vector<T>& v, const T& value)
		{
			return std::find(v.begin(), v.end(), value) != v.end();
		}

	} // namespace

	//--------------------------------------------------------------------------
	struct GridLayoutWidget::LabeledWidget
	{
		LabeledWidget(QString name,
		              GridLayoutWidget& parent,
		              QGridLayout& grid_layout,
		              int row,
		              const JsonReferenceResolver& jsonReferenceResolver,
		              QJsonValue json) :
			name(name)
		{
			label.setObjectName(get_unique_name("(LabeledWidget)label-"));

			grid_layout.addWidget(&label, row, 0);
			label.setText(tr("%1:").arg(name));
			jsonWidget = makeWidget(jsonReferenceResolver, json);
			grid_layout.addWidget(jsonWidget.get(), row, 1);
			QObject::connect(jsonWidget.get(),
			                 &IJsonWidget::hasChanged,
			                 &parent,
			                 &IJsonWidget::hasChanged);
		}

		QString name;
		QLabel label;
		std::unique_ptr<IJsonWidget> jsonWidget;
	};

	//--------------------------------------------------------------------------
	GridLayoutWidget::GridLayoutWidget(
		const JsonReferenceResolver& jsonReferenceResolver, QJsonValue json) :
		jsonReferenceResolver(jsonReferenceResolver),
		properties(json[json_keys::key_properties].toObject())
	{
		grid_layout_prop.setObjectName(
			get_unique_name("(GridLayoutWidget)gridLayoutProp-"));

		auto propertiesKeys = properties.keys();
		std::vector<QString> requiredKeys =
			toStrings(json[json_keys::key_required].toArray());
		const auto byPropertyOrderThenName = getPropertyKeyComparer(properties);

		std::sort(propertiesKeys.begin(),
		          propertiesKeys.end(),
		          byPropertyOrderThenName);
		std::sort(
			requiredKeys.begin(), requiredKeys.end(), byPropertyOrderThenName);

		for (const auto& key : requiredKeys)
		{
			jsonWidgets.push_back(
				std::make_unique<LabeledWidget>(key,
			                                    *this,
			                                    grid_layout_prop,
			                                    jsonWidgets.size(),
			                                    jsonReferenceResolver,
			                                    properties[key]));
		}

		QWidget* widget = new QWidget;
		widget->setLayout(&grid_layout_prop);
		tabWidget.addTab(widget, json[json_keys::key_title].toString());

		if (requiredKeys.size()
		        != static_cast<std::size_t>(propertiesKeys.size())
		    || json[json_keys::key_additionalProperties].toBool(true)
		    || json.toObject().contains(json_keys::key_patternProperties))
		{
			addOptionalProperties(json,
			                      requiredKeys,
			                      propertiesKeys,
			                      grid_layout_additional_prop);
			QWidget* widget_additional_prop = new QWidget;
			widget_additional_prop->setLayout(&grid_layout_additional_prop);
			tabWidget.addTab(widget_additional_prop, tr("Properties"));
			if (requiredKeys.empty()) { tabWidget.setCurrentIndex(1); }
		}
		layout.addWidget(&tabWidget);
		setLayout(&layout);
	}
	//--------------------------------------------------------------------------
	GridLayoutWidget::~GridLayoutWidget() /*override*/ = default;

	//--------------------------------------------------------------------------
	void GridLayoutWidget::addOptionalProperties(
		QJsonValue json,
		const std::vector<QString>& requiredKeys,
		QStringList propertiesKeys,
		QGridLayout& grid_layout)
	{
		std::vector<QString> optionalProperties;
		const auto byPropertyOrderThenName = getPropertyKeyComparer(properties);

		std::set_difference(propertiesKeys.begin(),
		                    propertiesKeys.end(),
		                    requiredKeys.begin(),
		                    requiredKeys.end(),
		                    std::back_inserter(optionalProperties),
		                    byPropertyOrderThenName);

		auto makeOnCheckBoxStateChanged = [this](auto key,
		                                         QCheckBox* checkBox) {
			return [key, checkBox, this](int state) {
				if (state == Qt::Checked)
				{
					const auto it = std::lower_bound(
						jsonWidgets.begin(),
						jsonWidgets.end(),
						key,
						[this](const auto& widget, QString key) {
							return getPropertyKeyComparer(properties)(
								widget->name, key);
						});
					const int rowCount = std::distance(jsonWidgets.begin(), it);
					insertRow(grid_layout_prop, rowCount);
					jsonWidgets.insert(
						it,
						std::make_unique<LabeledWidget>(key,
					                                    *this,
					                                    grid_layout_prop,
					                                    rowCount,
					                                    jsonReferenceResolver,
					                                    properties[key]));
				}
				else
				{
					auto it = std::find_if(
						jsonWidgets.begin(), jsonWidgets.end(), byName(key));
					if (it != jsonWidgets.end())
					{
						const int rowCount =
							std::distance(jsonWidgets.begin(), it);
						deleteRow(grid_layout_prop, rowCount);
						jsonWidgets.erase(it);
					}
				}
				emit this->hasChanged();
			};
		};

		for (const auto& key : optionalProperties)
		{
			auto checkBox = new QCheckBox(key, this);
			const auto row = grid_layout.rowCount();
			grid_layout.addWidget(checkBox, row, 0);
			auto desc = properties[key][json_keys::key_description].toString();
			if (!desc.isEmpty())
			{
				grid_layout.addWidget(new QLabel(desc, this), row, 1);
			}
			QObject::connect(checkBox,
			                 &QCheckBox::stateChanged,
			                 this,
			                 makeOnCheckBoxStateChanged(key, checkBox));
		}
		{
			const auto row = grid_layout.rowCount();
			auto* const lineEdit = new QLineEdit("", this);
			auto* const pushButton = new QPushButton(tr("Add"), this);
			pushButton->setEnabled(false);
			QObject::connect(lineEdit,
			                 &QLineEdit::textChanged,
			                 pushButton,
			                 [=](const QString& s) {
								 pushButton->setEnabled(
									 !s.isEmpty()
									 && !contains(optionalProperties, s)
									 && !contains(additional_prop_keys, s)
									 // TODO: pattern prop
								 );
							 });

			QObject::connect(
				pushButton, &QPushButton::clicked, pushButton, [=]() {
					auto key = lineEdit->text();

					auto checkBox = new QCheckBox(key, this);
					additional_prop_keys.push_back(key);
					const auto row =
						this->grid_layout_additional_prop.rowCount();
					this->grid_layout_additional_prop.addWidget(
						checkBox, row, 0);
					// TODO: delete button
					QObject::connect(checkBox,
				                     &QCheckBox::stateChanged,
				                     this,
				                     makeOnCheckBoxStateChanged(key, checkBox));

					lineEdit->setText("");
				});

			grid_layout.addWidget(lineEdit, row, 0);
			grid_layout.addWidget(pushButton, row, 1);
		}
		const auto dependencies = json[json_keys::key_dependencies].toObject();
		for (auto it = dependencies.constBegin(); it != dependencies.constEnd();
		     ++it)
		{
			const auto key_it = std::find(
				optionalProperties.begin(), optionalProperties.end(), it.key());
			if (key_it == optionalProperties.end())
			{
				// properties not found
				continue;
			}
			const auto index =
				std::distance(optionalProperties.begin(), key_it);
			const auto value = it.value();
			const auto children = value.isString()
			                        ? std::vector{value.toString()}
			                        : toStrings(value.toArray());

			std::vector<QCheckBox*> checkBoxes;
			for (const auto& child : children)
			{
				auto it2 = std::find(optionalProperties.begin(),
				                     optionalProperties.end(),
				                     child);
				if (it2 == optionalProperties.end())
				{
					// dependency not found
					break;
				}
				const auto index2 =
					std::distance(optionalProperties.begin(), it2);
				QCheckBox* w2 = dynamic_cast<QCheckBox*>(
					grid_layout.itemAtPosition(1 + index2, 0)->widget());
				if (w2) { checkBoxes.push_back(w2); }
			}

			QCheckBox* w1 = dynamic_cast<QCheckBox*>(
				grid_layout.itemAtPosition(1 + index, 0)->widget());

			w1->setEnabled(false);
			auto onStateChanged = [w1, checkBoxes]() {
				w1->setEnabled(std::all_of(checkBoxes.begin(),
				                           checkBoxes.end(),
				                           [](const QCheckBox* checkBox) {
											   return checkBox->isChecked();
										   }));
				if (!w1->isEnabled()) { w1->setChecked(false); }
			};
			for (auto* w : checkBoxes)
			{
				QObject::connect(
					w, &QCheckBox::stateChanged, w1, onStateChanged);
			}
		};
	}

	//--------------------------------------------------------------------------
	QJsonValue GridLayoutWidget::toQJson() const /*override*/
	{
		QJsonObject obj;
		for (const auto& labeledWidget : jsonWidgets)
		{
			obj[labeledWidget->name] = labeledWidget->jsonWidget->toQJson();
		}
		return obj;
	}
	//--------------------------------------------------------------------------
	void GridLayoutWidget::fromQJson(QJsonValue json) /*override*/
	{
		for (auto& labeledWidget : jsonWidgets)
		{
			labeledWidget->jsonWidget->fromQJson(json[labeledWidget->name]);
		}
	}

} // namespace object
