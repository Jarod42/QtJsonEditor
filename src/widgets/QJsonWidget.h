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
public:
	explicit QJsonWidget(QWidget* parent = nullptr);
	~QJsonWidget() noexcept override = default;

	QJsonValue getValue() const;

public slots:
	void setSchema(QJsonValue);
	void setValue(QJsonValue);

signals:
	void hasChanged();

private:
	QVBoxLayout layout;
	std::unique_ptr<IJsonWidget> widget;
	JsonReferenceResolver jsonReferenceResolver;
};
