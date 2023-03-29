#include "CalendarWidget.h"

#include "ui/EditorDialog.h"
#include "widgets/jsonKeys.h"

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
