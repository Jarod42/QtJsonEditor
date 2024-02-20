#include "QJsonWidget.h"

#include "QtJsonWidget/JsonReferenceResolver.h"
#include "QtJsonWidget/jsonKeys.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace
{
	//--------------------------------------------------------------------------
	QJsonValue parseToQJson(QString text, QJsonParseError& parseError)
	{
		// QJsonDocument only provides object or array getter :-/
		// wrap text in array
		auto doc =
			QJsonDocument::fromJson(("[" + text + "]").toUtf8(), &parseError);
		if (parseError.error != QJsonParseError::ParseError::NoError)
		{
			return {};
		}
		auto array = doc.array();
		if (array.size() != 1)
		{
			parseError.error = QJsonParseError::GarbageAtEnd;
			parseError.offset = text.size();
			return {};
		}

		return array[0];
	}

	//--------------------------------------------------------------------------
	QString toQString(QJsonValue json)
	{
		// QJsonDocument only provides constructor from object or array :-/
		// wrap value in array
		QJsonArray array;
		array.append(json);
		auto doc = QJsonDocument(array);
		QString s = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
		s.replace("\n    ", "\n"); // remove extra indentation
		s = s.trimmed();
		s.remove(0, 1); // remove '['
		s.remove(s.size() - 1, 1); // remove ']'
		s = s.trimmed();
		return s;
	}

} // namespace

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
QString QJsonWidget::getValueText() const
{
	return toQString(getValue());
}

//------------------------------------------------------------------------------
QString QJsonWidget::getSchemaText() const
{
	return schemaText;
}

//------------------------------------------------------------------------------
void QJsonWidget::setSchemaText(QString schema)
{
	if (schemaText != schema)
	{
		schemaText = schema;

		QJsonParseError parseError;
		auto json = parseToQJson(schema, parseError);

		if (parseError.error == QJsonParseError::NoError)
		{
			setSchemaImpl(json);
		}
	}
}

//------------------------------------------------------------------------------
void QJsonWidget::setSchema(QJsonValue schema)
{
	schemaText = toQString(schema);
	setSchemaImpl(schema);
}

//------------------------------------------------------------------------------
void QJsonWidget::setSchemaImpl(QJsonValue schema)
{
	jsonReferenceResolver.setSchemaRoot(schema);
	auto newWidget = makeWidget(jsonReferenceResolver, schema);
	if (widget) { layout.replaceWidget(widget.get(), newWidget.get()); }
	else { layout.addWidget(newWidget.get()); }
	widget = std::move(newWidget);
	QObject::connect(widget.get(),
	                 &IJsonWidget::hasChanged,
	                 this,
	                 &QJsonWidget::valueChanged);
	emit schemaChanged();
}

//------------------------------------------------------------------------------
void QJsonWidget::setValue(QJsonValue json)
{
	if (widget)
	{
		widget->fromQJson(json);
		emit valueChanged();
	}
}

//------------------------------------------------------------------------------
void QJsonWidget::setValueText(QString jsonText)
{
	if (widget)
	{
		QJsonParseError parseError;
		auto json = parseToQJson(jsonText, parseError);
		if (parseError.error == QJsonParseError::ParseError::NoError)
		{
			setValue(json);
		}
	}
}
