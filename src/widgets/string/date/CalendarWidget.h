#pragma once

#include "widgets/IJsonWidget.h"

#include <QCalendarWidget>

class CalendarWidget : public IJsonWidget
{
public:
	explicit CalendarWidget(QJsonValue json);

	QJsonValue toQJson() const override;
	void fromQJson(QJsonValue json) override;

	void changeEvent(QEvent*) override;

private:
	QHBoxLayout layout;
	QCalendarWidget calendarWidget;
};
