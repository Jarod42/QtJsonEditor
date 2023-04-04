#include "LabelWidget.h"

#include "widgets/jsonKeys.h"

namespace null
{

	//--------------------------------------------------------------------------
	LabelWidget::LabelWidget()
	{
		label.setObjectName(get_unique_name("label-"));
		label.setText("null");

		layout.addWidget(&label);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue LabelWidget::toQJson() const /*override*/
	{
		return QJsonValue(QJsonValue::Type::Null);
	}

	//--------------------------------------------------------------------------
	void LabelWidget::fromQJson(QJsonValue) /*override*/
	{
		// Empty
	}

} // namespace null
