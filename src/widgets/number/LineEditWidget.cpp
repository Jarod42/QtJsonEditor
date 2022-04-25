#include "LineEditWidget.h"

namespace number
{

	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
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
