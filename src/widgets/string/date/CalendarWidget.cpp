#include "CalendarWidget.h"

#include "widgets/jsonKeys.h"

#include <QEvent>
#include <QLocale>

//------------------------------------------------------------------------------
CalendarWidget::CalendarWidget(QJsonValue json)
{
	calendarWidget.setObjectName(get_unique_name("calendarWidget-"));

	calendarWidget.setMinimumDate(QDate::fromString(
		json[json_keys::key_minimum].toString(), Qt::DateFormat::ISODate));
	calendarWidget.setMaximumDate(QDate::fromString(
		json[json_keys::key_maximum].toString(), Qt::DateFormat::ISODate));
	calendarWidget.setSelectedDate(QDate::fromString(
		json[json_keys::key_default].toString(), Qt::DateFormat::ISODate));
	QObject::connect(&calendarWidget,
	                 &QCalendarWidget::selectionChanged,
	                 this,
	                 &IJsonWidget::hasChanged);
	layout.addWidget(&calendarWidget);
	setLayout(&layout);
}

//------------------------------------------------------------------------------
QJsonValue CalendarWidget::toQJson() const /*override*/
{
	return calendarWidget.selectedDate().toString(Qt::DateFormat::ISODate);
}

//------------------------------------------------------------------------------
void CalendarWidget::fromQJson(QJsonValue json) /*override*/
{
	QDate date = QDate::fromString(json.toString(), Qt::DateFormat::ISODate);
	calendarWidget.setSelectedDate(date);
}

//------------------------------------------------------------------------------
void CalendarWidget::changeEvent(QEvent* event) /*override*/
{
	if (event->type() == QEvent::LanguageChange)
	{
		calendarWidget.setLocale(QLocale());
	}
	QWidget::changeEvent(event);
}
