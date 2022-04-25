#pragma once

#include "widgets/IJsonWidget.h"

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
