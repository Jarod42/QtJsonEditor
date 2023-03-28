#include "DateEditWidget.h"

//------------------------------------------------------------------------------
DateEditWidget::DateEditWidget(QJsonValue json)
{
	dateEdit.setObjectName(get_unique_name("dateEdit-"));

	dateEdit.setMinimumDate(
		QDate::fromString(json["minimum"].toString(), Qt::DateFormat::ISODate));
	dateEdit.setMaximumDate(
		QDate::fromString(json["maximum"].toString(), Qt::DateFormat::ISODate));
	dateEdit.setDate(
		QDate::fromString(json["default"].toString(), Qt::DateFormat::ISODate));
	QObject::connect(
		&dateEdit, &QDateEdit::dateChanged, this, &IJsonWidget::hasChanged);
	layout.addWidget(&dateEdit);
	setLayout(&layout);
}

//------------------------------------------------------------------------------
QJsonValue DateEditWidget::toQJson() const /*override*/
{
	return dateEdit.date().toString(Qt::DateFormat::ISODate);
}

//------------------------------------------------------------------------------
void DateEditWidget::fromQJson(QJsonValue json) /*override*/
{
	QDate date = QDate::fromString(json.toString(), Qt::DateFormat::ISODate);
	dateEdit.setDate(date);
}
