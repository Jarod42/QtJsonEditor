#include "LineEditWidget.h"

#include "widgets/jsonKeys.h"

namespace integer
{
	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
		validator.setBottom(json[json_keys::key_minimum].toInt(
			std::numeric_limits<int>::min()));
		validator.setTop(json[json_keys::key_maximum].toInt(
			std::numeric_limits<int>::max()));

		lineEdit.setValidator(&validator);
		lineEdit.setText(QString::number(json[json_keys::key_default].toInt()));
		lineEdit.setPlaceholderText(
			json[json_keys::key_placeholder].toString());

		QObject::connect(
			&lineEdit, &QLineEdit::textChanged, this, &IJsonWidget::hasChanged);

		layout.addWidget(&lineEdit);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue LineEditWidget::toQJson() const /*override*/
	{
		return lineEdit.text().toInt();
	}

	//--------------------------------------------------------------------------
	void LineEditWidget::fromQJson(QJsonValue json) /*override*/
	{
		lineEdit.setText(QString::number(json.toInt()));
	}
} // namespace integer
