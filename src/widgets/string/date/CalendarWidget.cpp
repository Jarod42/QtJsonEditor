#include "CalendarWidget.h"

#include "ui/EditorDialog.h"

//------------------------------------------------------------------------------
CalendarWidget::CalendarWidget(QJsonValue json)
{
	calendarWidget.setObjectName(get_unique_name("calendarWidget-"));

	QDate date =
		QDate::fromString(json["default"].toString(), Qt::DateFormat::ISODate);
	calendarWidget.setSelectedDate(date);
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
