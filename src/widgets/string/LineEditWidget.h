#pragma once

#include "widgets/IJsonWidget.h"

#include <QLineEdit>

namespace string
{
	class LineEditWidget : public IJsonWidget
	{
	public:
		explicit LineEditWidget(QJsonValue json);

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue json) override;

	private:
		QHBoxLayout layout;
		QLineEdit lineEdit;
	};
} // namespace string
