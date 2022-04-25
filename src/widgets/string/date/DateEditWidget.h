#pragma once

#include "widgets/IJsonWidget.h"

#include <QDateEdit>

struct DateEditWidget : IJsonWidget
{
	explicit DateEditWidget(QJsonValue json);

	QJsonValue toQJson() const override;
	void fromQJson(QJsonValue json) override;

public:
	QHBoxLayout layout;
	QDateEdit dateEdit;
};
