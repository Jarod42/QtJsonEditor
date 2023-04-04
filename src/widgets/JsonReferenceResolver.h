#pragma once

#include <QJsonValue>

class JsonReferenceResolver
{
public:
	JsonReferenceResolver();

	void setSchemaRoot(QJsonValue);

	QJsonValue resolveReference(QString path) const;
	QJsonValue resolveReference(QString path, QJsonValue) const;

private:
	QJsonValue root;
};
