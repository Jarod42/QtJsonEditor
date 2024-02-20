#include "SpinBoxWidget.h"

#include "QtJsonWidget/jsonKeys.h"

namespace integer
{
	//--------------------------------------------------------------------------
	SpinBoxWidget::SpinBoxWidget(QJsonValue json)
	{
		spinBox.setObjectName(get_unique_name("spinBox-"));
		spinBox.setMinimum(json[json_keys::key_minimum].toInt());
		spinBox.setMaximum(json[json_keys::key_maximum].toInt());
		spinBox.setValue(json[json_keys::key_default].toInt());

		QObject::connect(
			&spinBox,
			static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
			this,
			&IJsonWidget::hasChanged);
		layout.addWidget(&spinBox);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue SpinBoxWidget::toQJson() const /*override*/
	{
		return spinBox.value();
	}

	//--------------------------------------------------------------------------
	void SpinBoxWidget::fromQJson(QJsonValue json) /*override*/
	{
		spinBox.setValue(json.toInt());
	}
} // namespace integer
