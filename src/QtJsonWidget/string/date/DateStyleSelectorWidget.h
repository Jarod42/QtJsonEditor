#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QComboBox>
#include <QEvent>
#include <QLabel>
#include <optional>

class DateStyleSelectorWidget : public IJsonWidget
{
public:
	explicit DateStyleSelectorWidget(QJsonValue schema);

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
	QLabel styleLabel;
	QComboBox styleComboBox;
	std::unique_ptr<IJsonWidget> widget;
};
