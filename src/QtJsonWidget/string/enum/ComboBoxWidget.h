#pragma once

#include "QtJsonWidget/IJsonWidget.h"

#include <QComboBox>

namespace string
{
	class ComboBoxWidget : public IJsonWidget
	{
	public:
		explicit ComboBoxWidget(QJsonValue json);

		QJsonValue toQJson() const override;
		void fromQJson(QJsonValue json) override;

	private:
		QHBoxLayout layout;
		QComboBox comboBox;
	};
} // namespace string
