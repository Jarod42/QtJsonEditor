#include "GridLayoutWidget.h"

#include <QJsonArray>

namespace array
{

	//--------------------------------------------------------------------------
	struct GridLayoutWidget::OrderedWidget
	{
		//----------------------------------------------------------------------
		OrderedWidget(GridLayoutWidget& parent,
		              QGridLayout& grid_layout,
		              QJsonValue json)
		{
			hLayout.setObjectName(get_unique_name("(OrderedWidget)hLayout-"));
			buttonDelete.setObjectName(
				get_unique_name("(OrderedWidget)buttonDelete-"));

			const auto row = grid_layout.rowCount();
			grid_layout.addLayout(&hLayout, row, 0);
			buttonDelete.setText("Delete");
			grid_layout.addWidget(&buttonDelete, row, 1);
			QObject::connect(&buttonDelete,
			                 &QPushButton::clicked,
			                 [this, &grid_layout, &parent]() {
								 auto it =
									 std::find_if(parent.jsonWidgets.begin(),
				                                  parent.jsonWidgets.end(),
				                                  [this](const auto& ptr) {
													  return ptr.get() == this;
												  });
								 if (it != parent.jsonWidgets.end())
								 {
									 parent.jsonWidgets.erase(it);
								 }
								 emit parent.hasChanged();
							 });
			jsonWidget = makeWidget(json, json["description"].toString());
			hLayout.addWidget(jsonWidget.get());
			QObject::connect(jsonWidget.get(),
			                 &IJsonWidget::hasChanged,
			                 &parent,
			                 &IJsonWidget::hasChanged);
		}

		QHBoxLayout hLayout;
		QPushButton buttonDelete;
		std::unique_ptr<IJsonWidget> jsonWidget;
	};

	//--------------------------------------------------------------------------
	GridLayoutWidget::GridLayoutWidget(QJsonValue jsonSchema) :
		jsonSchema(jsonSchema)
	{
		grid_layout.setObjectName(
			get_unique_name("(GridLayoutWidget)gridLayout-"));
		buttonNew.setObjectName(
			get_unique_name("(GridLayoutWidget)buttonNew-"));

		grid_layout.addWidget(&buttonNew, 0, 0, 1, 2);
		buttonNew.setText("Add");
		createItem = [&, this]() {
			jsonWidgets.push_back(std::make_unique<OrderedWidget>(
				*this, grid_layout, this->jsonSchema["items"]));
		};

		QObject::connect(&buttonNew, &QPushButton::clicked, [&, this]() {
			this->createItem();
			emit this->hasChanged();
		});
		setLayout(&grid_layout);
		auto initial = jsonSchema["default"];
		fromQJson(initial);
	}

	//--------------------------------------------------------------------------
	GridLayoutWidget::~GridLayoutWidget() /*override*/ = default;

	//--------------------------------------------------------------------------
	QJsonValue GridLayoutWidget::toQJson() const /*override*/
	{
		QJsonArray arr;
		for (const auto& jsonWidget : jsonWidgets)
		{
			arr.append(jsonWidget->jsonWidget->toQJson());
		}
		return arr;
	}
	void GridLayoutWidget::fromQJson(QJsonValue json) /*override*/
	{
		auto it = jsonWidgets.begin();
		for (const auto& inner : json.toArray())
		{
			if (it == jsonWidgets.end())
			{
				createItem();
				it = std::prev(jsonWidgets.end());
			}
			(*it)->jsonWidget->fromQJson(inner);
			++it;
		}
		jsonWidgets.erase(it, jsonWidgets.end());
	}
} // namespace array
