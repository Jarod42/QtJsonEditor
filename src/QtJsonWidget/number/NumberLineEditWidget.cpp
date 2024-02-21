#include "NumberLineEditWidget.h"

#include "QtJsonWidget/jsonKeys.h"

namespace number
{

	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
		validator.setBottom(json[json_keys::key_minimum].toDouble(
			std::numeric_limits<double>::min()));
		validator.setTop(json[json_keys::key_maximum].toDouble(
			std::numeric_limits<double>::max()));

		lineEdit.setValidator(&validator);
		lineEdit.setText(
			QString::number(json[json_keys::key_default].toDouble()));
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
		return QLocale().toDouble(lineEdit.text());
	}

	//--------------------------------------------------------------------------
	void LineEditWidget::fromQJson(QJsonValue json) /*override*/
	{
		lineEdit.setText(QString::number(json.toDouble()));
	}

} // namespace number
