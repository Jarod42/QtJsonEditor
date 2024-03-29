#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QGridLayout>
#include <QPushButton>
#include <functional>
#include <memory>
#include <vector>

namespace array
{

	class GridLayoutWidget : public IJsonWidget
	{
	public:
		GridLayoutWidget(const JsonReferenceResolver&, QJsonValue);
		~GridLayoutWidget() override;

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

		void changeEvent(QEvent*) override;

		void retranslateUi();

	private:
		struct OrderedWidget;

		std::function<void()> createItem;
		QJsonValue jsonSchema;
		QGridLayout grid_layout;
		QPushButton buttonNew;
		std::vector<std::unique_ptr<OrderedWidget>> jsonWidgets;
	};

} // namespace array
