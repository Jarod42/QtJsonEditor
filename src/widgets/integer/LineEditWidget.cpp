#include "LineEditWidget.h"

namespace integer
{
	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
		validator.setBottom(
			json["minimum"].toInt(std::numeric_limits<int>::min()));
		validator.setTop(
			json["maximum"].toInt(std::numeric_limits<int>::max()));

		lineEdit.setValidator(&validator);
		lineEdit.setText(QString::number(json["default"].toInt()));
		lineEdit.setPlaceholderText(json["placeholder"].toString());

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
