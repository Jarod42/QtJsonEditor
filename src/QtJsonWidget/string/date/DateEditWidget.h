#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QDateEdit>

struct DateEditWidget : IJsonWidget
{
	explicit DateEditWidget(QJsonValue json);

	QJsonValue toQJson() const override;
	void fromQJson(QJsonValue json) override;

	void changeEvent(QEvent*) override;

public:
	QHBoxLayout layout;
	QDateEdit dateEdit;
};
