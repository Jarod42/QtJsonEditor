#pragma once

#include "widgets/IJsonWidget.h"

#include <QColor>
#include <QPushButton>

class ColorDialogButtonWidget : public IJsonWidget
{
public:
	explicit ColorDialogButtonWidget(QJsonValue);

	QJsonValue toQJson() const override;
	void fromQJson(QJsonValue) override;

public:
	void SelectColor();

private:
	QHBoxLayout layout;
	QColor color;
	QPushButton button;
};
