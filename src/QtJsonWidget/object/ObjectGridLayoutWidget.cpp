#include "ObjectGridLayoutWidget.h"

#include "QtJsonWidget/jsonKeys.h"

#include <QCheckBox>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTabBar>

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
		template <typename Range, typename T>
		bool contains(const Range& r, const T& value)
		{
			return std::find(r.begin(), r.end(), value) != r.end();
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
			jsonWidget = makeWidget(jsonReferenceResolver, json);
			grid_layout.addWidget(jsonWidget.get(), row, 1);
			QObject::connect(jsonWidget.get(),
			                 &IJsonWidget::hasChanged,
			                 &parent,
			                 &IJsonWidget::hasChanged);
			retranslateUi();
		}

		void retranslateUi()
		{
			label.setText(IJsonWidget::tr("%1:").arg(name));
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
			tabWidget.addTab(widget_additional_prop, "");
			if (requiredKeys.empty()) { tabWidget.setCurrentIndex(1); }
		}
		layout.addWidget(&tabWidget);
		setLayout(&layout);
		retranslateUi();
	}
	//--------------------------------------------------------------------------
	GridLayoutWidget::~GridLayoutWidget() /*override*/ = default;

	//--------------------------------------------------------------------------
	void GridLayoutWidget::changeEvent(QEvent* event) /*override*/
	{
		if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
		QWidget::changeEvent(event);
	}

	//--------------------------------------------------------------------------
	void GridLayoutWidget::retranslateUi()
	{
		if (tabWidget.tabBar()->count() == 2)
		{
			tabWidget.setTabText(1, IJsonWidget::tr("Properties"));
		}
		if (newPropertyPushButton)
		{
			newPropertyPushButton->setText(IJsonWidget::tr("Add"));
		}
		for (auto& w : jsonWidgets)
		{
			w->retranslateUi();
		}
	}

	//--------------------------------------------------------------------------
	void GridLayoutWidget::addItem(const QString& key)
	{
		const auto it = std::lower_bound(
			jsonWidgets.begin(),
			jsonWidgets.end(),
			key,
			[this](const auto& widget, QString key) {
				return getPropertyKeyComparer(properties)(widget->name, key);
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

	//--------------------------------------------------------------------------
	void GridLayoutWidget::onCheckBoxStateChanged(const QString& key, int state)
	{
		if (state == Qt::Checked) { addItem(key); }
		else
		{
			auto it = std::find_if(
				jsonWidgets.begin(), jsonWidgets.end(), byName(key));
			if (it != jsonWidgets.end())
			{
				const int rowCount = std::distance(jsonWidgets.begin(), it);
				deleteRow(grid_layout_prop, rowCount);
				jsonWidgets.erase(it);
			}
		}
		emit this->hasChanged();
	}

	//--------------------------------------------------------------------------
	void GridLayoutWidget::addExtraProperty(const QString& key)
	{
		auto checkBox = new QCheckBox(key, this);
		allCheckBoxes.push_back(checkBox);
		additional_prop_keys.push_back(key);
		const auto row = this->grid_layout_additional_prop.rowCount();
		this->grid_layout_additional_prop.addWidget(checkBox, row, 0);
		// TODO: delete button
		QObject::connect(
			checkBox, &QCheckBox::stateChanged, this, [this, key](int state) {
				return onCheckBoxStateChanged(key, state);
			});
	}

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

		for (const auto& key : optionalProperties)
		{
			auto checkBox = new QCheckBox(key, this);
			allCheckBoxes.push_back(checkBox);
			const auto row = grid_layout.rowCount();
			grid_layout.addWidget(checkBox, row, 0);
			auto desc = properties[key][json_keys::key_description].toString();
			if (!desc.isEmpty())
			{
				grid_layout.addWidget(new QLabel(desc, this), row, 1);
			}
			QObject::connect(
				checkBox,
				&QCheckBox::stateChanged,
				this,
				[this, key](int state) { onCheckBoxStateChanged(key, state); });
		}
		{
			const auto row = grid_layout.rowCount();
			auto* const lineEdit = new QLineEdit("", this);
			newPropertyPushButton = new QPushButton("", this);
			newPropertyPushButton->setEnabled(false);
			QObject::connect(lineEdit,
			                 &QLineEdit::textChanged,
			                 newPropertyPushButton,
			                 [=](const QString& s) {
								 newPropertyPushButton->setEnabled(
									 !s.isEmpty()
									 && !contains(optionalProperties, s)
									 && !contains(additional_prop_keys, s)
									 // TODO: pattern prop
								 );
							 });

			QObject::connect(newPropertyPushButton,
			                 &QPushButton::clicked,
			                 this,
			                 [=]() {
								 auto key = lineEdit->text();

								 addExtraProperty(key);
								 lineEdit->setText("");
							 });

			grid_layout.addWidget(lineEdit, row, 0);
			grid_layout.addWidget(newPropertyPushButton, row, 1);
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

			std::vector<QCheckBox*> dependentCheckBoxes;
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
				if (w2) { dependentCheckBoxes.push_back(w2); }
			}

			QCheckBox* w1 = dynamic_cast<QCheckBox*>(
				grid_layout.itemAtPosition(1 + index, 0)->widget());

			w1->setEnabled(false);
			auto onStateChanged = [w1, dependentCheckBoxes]() {
				w1->setEnabled(std::all_of(dependentCheckBoxes.begin(),
				                           dependentCheckBoxes.end(),
				                           [](const QCheckBox* checkBox) {
											   return checkBox->isChecked();
										   }));
				if (!w1->isEnabled()) { w1->setChecked(false); }
			};
			for (auto* w : dependentCheckBoxes)
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
		const auto byPropertyOrderThenName = getPropertyKeyComparer(properties);
		auto keys = json.toObject().keys();

		std::sort(keys.begin(), keys.end(), byPropertyOrderThenName);

		for (const auto& key : keys)
		{
			auto it = std::find_if(
				jsonWidgets.begin(), jsonWidgets.end(), byName(key));
			if (it == jsonWidgets.end())
			{
				auto it2 = std::find_if(
					allCheckBoxes.begin(),
					allCheckBoxes.end(),
					[&](const auto* c) { return c->text() == key; });
				if (it2 == allCheckBoxes.end())
				{
					addExtraProperty(key);
					it2 = std::find_if(
						allCheckBoxes.begin(),
						allCheckBoxes.end(),
						[&](const auto* c) { return c->text() == key; });
				}
				(*it2)->setChecked(true);

				it = std::find_if(
					jsonWidgets.begin(), jsonWidgets.end(), byName(key));
			}
			(*it)->jsonWidget->fromQJson(json[key]);
		}
		for (auto* c : allCheckBoxes)
		{
			if (!contains(keys, c->text())) { c->setChecked(false); }
		}
	}

} // namespace object
