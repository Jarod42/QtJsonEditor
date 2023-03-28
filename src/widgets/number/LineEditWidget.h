#pragma once

#include "widgets/IJsonWidget.h"

#include <QDoubleValidator>
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
		QDoubleValidator validator;
	};
} // namespace number
