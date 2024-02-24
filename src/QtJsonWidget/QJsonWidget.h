#pragma once

#include "IJsonWidget.h"
#include "JsonReferenceResolver.h"

#include <QBoxLayout>
#include <QJsonValue>
#include <QWidget>
#include <memory>

class QJsonWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QString schema READ getSchemaText WRITE setSchemaText NOTIFY schemaChanged USER true)
	Q_PROPERTY(QString value READ getValueText WRITE setValueText NOTIFY valueChanged)

public:
	explicit QJsonWidget(QWidget* parent = nullptr);
	~QJsonWidget() noexcept override = default;

	QString getSchemaText() const;
	QString getValueText() const;
	QJsonValue getValue() const;

public slots:
	void setSchemaText(QString);
	void setSchema(QJsonValue);
	void setValueText(QString);
	void setValue(QJsonValue);

signals:
	void schemaChanged();
	void valueChanged();

private:
	void setSchemaImpl(QJsonValue);

private:
	QString schemaText;
	QVBoxLayout layout;
	std::unique_ptr<IJsonWidget> widget;
	JsonReferenceResolver jsonReferenceResolver;
};
