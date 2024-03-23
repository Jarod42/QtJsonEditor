#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QComboBox>
#include <QEvent>
#include <QLabel>

class StringFormatSelectorWidget : public IJsonWidget
{
public:
	explicit StringFormatSelectorWidget(QJsonValue schema);

	QJsonValue toQJson() const override { return widget->toQJson(); }
	void fromQJson(QJsonValue value) override { widget->fromQJson(value); }
	void changeEvent(QEvent* event) override;

	void retranslateUi();

private:
	void replaceWidget();

private:
	QJsonValue schema;
	QVBoxLayout vLayout;
	QHBoxLayout hLayout;
	QLabel formatLabel;
	QComboBox formatComboBox;
	std::unique_ptr<IJsonWidget> widget;
};
