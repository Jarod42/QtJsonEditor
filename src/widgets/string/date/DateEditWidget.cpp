#include "DateEditWidget.h"

#include "widgets/jsonKeys.h"

#include <QEvent>
#include <QLocale>

//------------------------------------------------------------------------------
DateEditWidget::DateEditWidget(QJsonValue json)
{
	dateEdit.setObjectName(get_unique_name("dateEdit-"));

	dateEdit.setMinimumDate(QDate::fromString(
		json[json_keys::key_minimum].toString(), Qt::DateFormat::ISODate));
	dateEdit.setMaximumDate(QDate::fromString(
		json[json_keys::key_maximum].toString(), Qt::DateFormat::ISODate));
	dateEdit.setDate(QDate::fromString(json[json_keys::key_default].toString(),
	                                   Qt::DateFormat::ISODate));
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

//------------------------------------------------------------------------------
void DateEditWidget::changeEvent(QEvent* event) /*override*/
{
	if (event->type() == QEvent::LanguageChange)
	{
		dateEdit.setDisplayFormat(QLocale().dateFormat(QLocale::ShortFormat));
		dateEdit.setLocale(QLocale());
	}
	QWidget::changeEvent(event);
}
