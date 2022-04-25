#include "LineEditWidget.h"

namespace integer
{
	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
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
