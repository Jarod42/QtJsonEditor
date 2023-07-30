#pragma once

#include "widgets/IJsonWidget.h"

#include <QCheckBox>
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
		void addItem(const QString& key);
		void addExtraProperty(const QString& key);
		void onCheckBoxStateChanged(const QString& key, int state);
		void addOptionalProperties(QJsonValue json,
		                           const std::vector<QString>& requiredKeys,
		                           QStringList propertiesKeys,
		                           QGridLayout& grid_layout);

	private:
		const JsonReferenceResolver& jsonReferenceResolver;
		const QJsonObject properties;
		std::vector<QCheckBox*> allCheckBoxes;
		QTabWidget tabWidget;
		QVBoxLayout layout;
		QGridLayout grid_layout_prop;
		QGridLayout grid_layout_additional_prop;
		std::vector<std::unique_ptr<LabeledWidget>> jsonWidgets;
		std::vector<QString> additional_prop_keys;
	};
} // namespace object
