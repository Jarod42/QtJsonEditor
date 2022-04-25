#include "SpinBoxWidget.h"

namespace integer
{
	//--------------------------------------------------------------------------
	SpinBoxWidget::SpinBoxWidget(QJsonValue json)
	{
		spinBox.setObjectName(get_unique_name("spinBox-"));
		spinBox.setMinimum(json["minimum"].toInt());
		spinBox.setMaximum(json["maximum"].toInt());
		spinBox.setValue(json["default"].toInt());

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
