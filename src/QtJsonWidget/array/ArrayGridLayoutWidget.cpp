#include "ArrayGridLayoutWidget.h"

#include "QtJsonWidget/jsonKeys.h"

#include <QEvent>
#include <QJsonArray>

namespace array
{

	//--------------------------------------------------------------------------
	struct GridLayoutWidget::OrderedWidget
	{
		//----------------------------------------------------------------------
		OrderedWidget(GridLayoutWidget& parent,
		              QGridLayout& grid_layout,
		              const JsonReferenceResolver& jsonReferenceResolver,
		              QJsonValue json)
		{
			hLayout.setObjectName(get_unique_name("(OrderedWidget)hLayout-"));
			buttonDelete.setObjectName(
				get_unique_name("(OrderedWidget)buttonDelete-"));

			const auto row = grid_layout.rowCount();
			grid_layout.addLayout(&hLayout, row, 0);
			grid_layout.addWidget(&buttonDelete, row, 1);
			QObject::connect(&buttonDelete,
			                 &QPushButton::clicked,
			                 [this, &parent]() {
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
			jsonWidget = makeWidget(jsonReferenceResolver, json);
			hLayout.addWidget(jsonWidget.get());
			QObject::connect(jsonWidget.get(),
			                 &IJsonWidget::hasChanged,
			                 &parent,
			                 &IJsonWidget::hasChanged);
			retranslateUi();
		}

		void retranslateUi()
		{
			buttonDelete.setText(IJsonWidget::tr("Delete"));
		}

		QHBoxLayout hLayout;
		QPushButton buttonDelete;
		std::unique_ptr<IJsonWidget> jsonWidget;
	};

	//--------------------------------------------------------------------------
	GridLayoutWidget::GridLayoutWidget(
		const JsonReferenceResolver& jsonReferenceResolver,
		QJsonValue jsonSchema) :
		jsonSchema(jsonSchema)
	{
		grid_layout.setObjectName(
			get_unique_name("(GridLayoutWidget)gridLayout-"));
		buttonNew.setObjectName(
			get_unique_name("(GridLayoutWidget)buttonNew-"));

		grid_layout.addWidget(&buttonNew, 0, 0, 1, 2);
		createItem = [&, this]() {
			jsonWidgets.push_back(std::make_unique<OrderedWidget>(
				*this,
				grid_layout,
				jsonReferenceResolver,
				this->jsonSchema[json_keys::key_items]));
		};

		QObject::connect(&buttonNew, &QPushButton::clicked, [&, this]() {
			this->createItem();
			emit this->hasChanged();
		});
		setLayout(&grid_layout);
		auto initial = jsonSchema[json_keys::key_default];
		fromQJson(initial);
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
		buttonNew.setText(IJsonWidget::tr("Add"));
		for (auto& w : jsonWidgets)
		{
			w->retranslateUi();
		}
	}

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
