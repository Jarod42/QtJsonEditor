#include "GridLayoutWidget.h"

#include <QJsonObject>
#include <QLabel>

namespace object
{

	//--------------------------------------------------------------------------
	struct GridLayoutWidget::LabeledWidget
	{
		LabeledWidget(QString name,
		              GridLayoutWidget& parent,
		              QGridLayout& grid_layout,
		              QJsonValue json) :
			name(name)
		{
			label.setObjectName(get_unique_name("(LabeledWidget)label-"));

			const auto row = grid_layout.rowCount();
			grid_layout.addWidget(&label, row, 0);
			label.setText(name + ":");
			jsonWidget = makeWidget(json, json["description"].toString());
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
	GridLayoutWidget::GridLayoutWidget(QJsonValue json)
	{
		grid_layout.setObjectName(
			get_unique_name("(GridLayoutWidget)gridLayout-"));
		const auto properties = json["properties"].toObject();

		auto sortedKeys = properties.keys();
		const auto byPropertyOrderThenName = [&](const QString& lhs,
		                                         const QString& rhs) {
			return std::forward_as_tuple(
					   properties[lhs]["propertyOrder"].toInt(1000), lhs)
			     < std::forward_as_tuple(
					   properties[rhs]["propertyOrder"].toInt(1000), rhs);
		};
		std::sort(
			sortedKeys.begin(), sortedKeys.end(), byPropertyOrderThenName);

		for (const auto& key : sortedKeys)
		{
			jsonWidgets.push_back(std::make_unique<LabeledWidget>(
				key, *this, grid_layout, properties[key]));
		}
		setLayout(&grid_layout);
	}
	//--------------------------------------------------------------------------
	GridLayoutWidget::~GridLayoutWidget() /*override*/ = default;

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
		auto it = jsonWidgets.begin();
		for (const auto& inner : json.toObject())
		{
			(*it)->jsonWidget->fromQJson(inner);
			++it;
			if (it == jsonWidgets.end()) { return; }
		}
	}

} // namespace object
