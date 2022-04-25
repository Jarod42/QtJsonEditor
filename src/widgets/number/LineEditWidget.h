#pragma once

#include "widgets/IJsonWidget.h"

#include <QLineEdit>

namespace number
{
	class LineEditWidget : public IJsonWidget
	{
	public:
		explicit LineEditWidget(QJsonValue);

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

	private:
		QHBoxLayout layout;
		QLineEdit lineEdit;
	};
} // namespace number
