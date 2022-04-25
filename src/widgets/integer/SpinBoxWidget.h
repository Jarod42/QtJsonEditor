#pragma once

#include "widgets/IJsonWidget.h"

#include <QSpinBox>

namespace integer
{
	class SpinBoxWidget : public IJsonWidget
	{
	public:
		explicit SpinBoxWidget(QJsonValue);

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

	private:
		QHBoxLayout layout;
		QSpinBox spinBox;
	};
} // namespace integer
