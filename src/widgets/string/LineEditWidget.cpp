#include "LineEditWidget.h"

namespace string
{
	//--------------------------------------------------------------------------
	LineEditWidget::LineEditWidget(QJsonValue json)
	{
		lineEdit.setObjectName(get_unique_name("lineEdit-"));
		lineEdit.setText(json["default"].toString());
		lineEdit.setPlaceholderText(json["placeholder"].toString());

		QObject::connect(
			&lineEdit, &QLineEdit::textChanged, this, &IJsonWidget::hasChanged);
		layout.addWidget(&lineEdit);
		setLayout(&layout);
	}

	//--------------------------------------------------------------------------
	QJsonValue LineEditWidget::toQJson() const /*override*/
	{
		return lineEdit.text();
	}

	//--------------------------------------------------------------------------
	void LineEditWidget::fromQJson(QJsonValue json) /*override*/
	{
		lineEdit.setText(json.toString());
	}

} // namespace string
