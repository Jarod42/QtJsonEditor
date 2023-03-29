#include "QJsonWidget.h"

#include "widgets/jsonKeys.h"

//------------------------------------------------------------------------------
QJsonWidget::QJsonWidget(QWidget* parent) : QWidget(parent)
{
	setLayout(&layout);
}

//------------------------------------------------------------------------------
QJsonValue QJsonWidget::getValue() const
{
	return widget ? widget->toQJson() : QJsonValue{};
}

//------------------------------------------------------------------------------
void QJsonWidget::setSchema(QJsonValue schema)
{
	if (widget)
	{
		layout.removeWidget(widget.get());
		const auto oldValue = widget->toQJson();
		widget =
			makeWidget(schema, schema[json_keys::key_description].toString());
		widget->fromQJson(oldValue);
	}
	else
	{
		widget =
			makeWidget(schema, schema[json_keys::key_description].toString());
	}
	QObject::connect(
		widget.get(), &IJsonWidget::hasChanged, this, &QJsonWidget::hasChanged);
	emit hasChanged();
	layout.addWidget(widget.get());
}

//------------------------------------------------------------------------------
void QJsonWidget::setValue(QJsonValue json)
{
	if (widget) { widget->fromQJson(json); }
}
