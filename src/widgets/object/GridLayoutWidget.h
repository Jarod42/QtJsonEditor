#pragma once

#include "widgets/IJsonWidget.h"

#include <QJsonObject>
#include <QTabWidget>

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
		void addOptionalProperties(QJsonValue json,
		                           const std::vector<QString>& requiredKeys,
		                           QStringList propertiesKeys,
		                           QGridLayout& grid_layout);

	private:
		const JsonReferenceResolver& jsonReferenceResolver;
		const QJsonObject properties;
		QTabWidget tabWidget;
		QVBoxLayout layout;
		QGridLayout grid_layout_prop;
		QGridLayout grid_layout_additional_prop;
		std::vector<std::unique_ptr<LabeledWidget>> jsonWidgets;
	};
} // namespace object
