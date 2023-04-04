#pragma once

#include "widgets/IJsonWidget.h"

namespace object
{

	class GridLayoutWidget : public IJsonWidget
	{
	public:
		GridLayoutWidget(const JsonReferenceResolver&, QJsonValue);
		~GridLayoutWidget() override;
		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

	private:
		struct LabeledWidget;

	private:
		QGridLayout grid_layout;
		std::vector<std::unique_ptr<LabeledWidget>> jsonWidgets;
	};
} // namespace object
