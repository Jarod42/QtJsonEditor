#include "CheckBoxWidget.h"

#include "widgets/jsonKeys.h"

namespace boolean
{

	//--------------------------------------------------------------------------
	CheckBoxWidget::CheckBoxWidget(QJsonValue json)
	{
		checkBox.setObjectName(get_unique_name("checkBox-"));
		checkBox.setChecked(json[json_keys::key_default].toBool());
		QObject::connect(&checkBox,
		                 &QCheckBox::stateChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
		layout.addWidget(&checkBox);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue CheckBoxWidget::toQJson() const /*override*/
	{
		return checkBox.isChecked();
	}

	//--------------------------------------------------------------------------
	void CheckBoxWidget::fromQJson(QJsonValue json) /*override*/
	{
		checkBox.setChecked(json.toBool());
	}

} // namespace boolean
