#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QLabel>

namespace null
{
	class LabelWidget : public IJsonWidget
	{
	public:
		explicit LabelWidget();

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

	private:
		QHBoxLayout layout;
		QLabel label;
	};
} // namespace null
