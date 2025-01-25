#pragma once

#include <QBoxLayout>
#include <QJsonValue>
#include <QString>
#include <QWidget>
#include <memory>

class JsonReferenceResolver;

class IJsonWidget : public QWidget
{
	Q_OBJECT
public:
	~IJsonWidget() noexcept override = default;

	virtual QJsonValue toQJson() const = 0;
	virtual void fromQJson(QJsonValue) = 0;

Q_SIGNALS:
	void hasChanged();
};

enum class EDescription
{
	Without,
	With
};

std::unique_ptr<IJsonWidget> makeWidget(const JsonReferenceResolver&,
                                        QJsonValue,
                                        EDescription = EDescription::With);

inline QString get_unique_name(const QString& prefix)
{
	static std::size_t counter = 0;
	return prefix + QString::number(counter++);
}
