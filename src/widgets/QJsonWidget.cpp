#include "QJsonWidget.h"

#include "widgets/JsonReferenceResolver.h"
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
	jsonReferenceResolver.setSchemaRoot(schema);
	auto newWidget = makeWidget(jsonReferenceResolver, schema);
	if (widget) { layout.replaceWidget(widget.get(), newWidget.get()); }
	else { layout.addWidget(newWidget.get()); }
	widget = std::move(newWidget);
	QObject::connect(
		widget.get(), &IJsonWidget::hasChanged, this, &QJsonWidget::hasChanged);
	emit hasChanged();
}

//------------------------------------------------------------------------------
void QJsonWidget::setValue(QJsonValue json)
{
	if (widget) { widget->fromQJson(json); }
}
