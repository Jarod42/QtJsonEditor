#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QCheckBox>

namespace boolean
{

	class CheckBoxWidget : public IJsonWidget
	{
	public:
		explicit CheckBoxWidget(QJsonValue);

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue) override;

	private:
		QHBoxLayout layout;
		QCheckBox checkBox;
	};

} // namespace boolean
