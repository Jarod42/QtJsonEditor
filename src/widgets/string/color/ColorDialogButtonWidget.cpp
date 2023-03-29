#include "ColorDialogButtonWidget.h"

#include "widgets/jsonKeys.h"

#include <QColorDialog>

//------------------------------------------------------------------------------
ColorDialogButtonWidget::ColorDialogButtonWidget(QJsonValue json)
{
	button.setObjectName(get_unique_name("pushButton-"));
	color.setNamedColor(json[json_keys::key_default].toString());
	button.setText(json[json_keys::key_default].toString());
	QObject::connect(
		&button, &QPushButton::clicked, [this]() { SelectColor(); });
	layout.addWidget(&button);
	setLayout(&layout);
}

//------------------------------------------------------------------------------
QJsonValue ColorDialogButtonWidget::toQJson() const /*override*/
{
	return color.name();
}
//------------------------------------------------------------------------------
void ColorDialogButtonWidget::fromQJson(QJsonValue json) /*override*/
{
	color.setNamedColor(json.toString());
	button.setText(json.toString());
}

//------------------------------------------------------------------------------
void ColorDialogButtonWidget::SelectColor()
{
	color = QColorDialog::getColor(color, &button, QString());
	button.setText(color.name());
	emit this->hasChanged();
}
