#include "DateStyleSelectorWidget.h"

#include "QtJsonWidget/string/date/CalendarWidget.h"
#include "QtJsonWidget/string/date/DateEditWidget.h"
#include "QtJsonWidget/jsonKeys.h"

namespace
{

	//----------------------------------------------------------------------
	enum class EStyle
	{
		Calendar,
		DateEdit,
	};

	//----------------------------------------------------------------------
	QString tr(EStyle style)
	{
		switch (style)
		{
			case EStyle::Calendar: return IJsonWidget::tr("Calendar");
			case EStyle::DateEdit: return IJsonWidget::tr("Date edit");
		}
		return "";
	}

	//----------------------------------------------------------------------
	std::optional<EStyle> toEStyle(QString s)
	{
		if (s == json_keys::style_calendar) { return EStyle::Calendar; }
		else if (s == json_keys::style_dateedit) { return EStyle::DateEdit; }
		else { return std::nullopt; }
	}

	//--------------------------------------------------------------------------
	std::unique_ptr<IJsonWidget> makeWidget(EStyle integerStyle,
	                                        QJsonValue schema)
	{
		switch (integerStyle)
		{
			default:
			case EStyle::Calendar:
				return std::make_unique<CalendarWidget>(schema);
			case EStyle::DateEdit:
				return std::make_unique<DateEditWidget>(schema);
		}
	}
} // namespace

//--------------------------------------------------------------------------
DateStyleSelectorWidget::DateStyleSelectorWidget(QJsonValue schema) :
	schema(schema)
{
	styleComboBox.addItem("", int(EStyle::Calendar));
	styleComboBox.addItem("", int(EStyle::DateEdit));

	const auto style = toEStyle(schema[json_keys::key_style].toString());
	constexpr auto defaultStyle = EStyle::Calendar;
	widget = makeWidget(style.value_or(defaultStyle), schema);
	styleComboBox.setCurrentIndex(
		styleComboBox.findData(static_cast<int>(style.value_or(defaultStyle))));
	if (style)
	{
		styleComboBox.hide();
		styleLabel.hide();
	}

	hLayout.addWidget(&styleLabel);
	hLayout.addWidget(&styleComboBox);

	vLayout.addLayout(&hLayout);
	vLayout.addWidget(widget.get());

	QObject::connect(
		&styleComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,
		&DateStyleSelectorWidget::replaceWidget);

	setLayout(&vLayout);
	retranslateUi();
}

//--------------------------------------------------------------------------
void DateStyleSelectorWidget::changeEvent(QEvent* event) /* override */
{
	if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
	QWidget::changeEvent(event);
}

//--------------------------------------------------------------------------
void DateStyleSelectorWidget::retranslateUi()
{
	styleLabel.setText(IJsonWidget::tr("style:"));
	for (int i = 0; i != styleComboBox.count(); ++i)
	{
		styleComboBox.setItemText(
			i, ::tr(EStyle(styleComboBox.itemData(i).toInt())));
	}
}

//--------------------------------------------------------------------------
void DateStyleSelectorWidget::replaceWidget()
{
	const auto integerStyle = static_cast<EStyle>(
		styleComboBox.itemData(styleComboBox.currentIndex()).toInt());

	auto newWidget = makeWidget(integerStyle, schema);
	newWidget->fromQJson(widget->toQJson());
	vLayout.replaceWidget(widget.get(), newWidget.get());
	widget = std::move(newWidget);
	QObject::connect(
		widget.get(), &IJsonWidget::hasChanged, this, &IJsonWidget::hasChanged);
}
