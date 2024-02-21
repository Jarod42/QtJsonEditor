#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QIntValidator>
#include <QLineEdit>

namespace integer
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
		QIntValidator validator;
	};

} // namespace integer
