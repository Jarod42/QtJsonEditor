#include "ComboBoxWidget.h"

#include <QJsonArray>

namespace string
{
	//--------------------------------------------------------------------------
	ComboBoxWidget::ComboBoxWidget(QJsonValue json)
	{
		comboBox.setObjectName(get_unique_name("comboBox-"));
		for (const auto& text : json["enum"].toArray())
		{
			comboBox.addItem(text.toString());
		}
		comboBox.setCurrentText(json["default"].toString());
		QObject::connect(&comboBox,
		                 static_cast<void (QComboBox::*)(int)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &IJsonWidget::hasChanged);
		layout.addWidget(&comboBox);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue ComboBoxWidget::toQJson() const /*override*/
	{
		return comboBox.currentText();
	}

	//--------------------------------------------------------------------------
	void ComboBoxWidget::fromQJson(QJsonValue json) /*override*/
	{
		comboBox.setCurrentText(json.toString());
	}

} // namespace string
