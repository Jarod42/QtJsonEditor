#include "LineEditWidget.h"

namespace number
{

	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
		validator.setBottom(
			json["minimum"].toDouble(std::numeric_limits<double>::min()));
		validator.setTop(
			json["maximum"].toDouble(std::numeric_limits<double>::max()));

		lineEdit.setValidator(&validator);
		lineEdit.setText(QString::number(json["default"].toDouble()));
		lineEdit.setPlaceholderText(json["placeholder"].toString());
		QObject::connect(
			&lineEdit, &QLineEdit::textChanged, this, &IJsonWidget::hasChanged);

		layout.addWidget(&lineEdit);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue LineEditWidget::toQJson() const /*override*/
	{
		return lineEdit.text().toDouble();
	}

	//--------------------------------------------------------------------------
	void LineEditWidget::fromQJson(QJsonValue json) /*override*/
	{
		lineEdit.setText(QString::number(json.toDouble()));
	}

} // namespace number
