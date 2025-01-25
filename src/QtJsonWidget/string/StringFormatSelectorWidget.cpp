#include "StringFormatSelectorWidget.h"

#include "QtJsonWidget/jsonKeys.h"
#include "QtJsonWidget/string/StringLineEditWidget.h"
#include "QtJsonWidget/string/color/ColorDialogButtonWidget.h"
#include "QtJsonWidget/string/date/DateStyleSelectorWidget.h"

namespace
{
	//--------------------------------------------------------------------------
	enum class EFormat
	{
		String,
		Color,
		Date
	};

	//--------------------------------------------------------------------------
	QString tr(EFormat format)
	{
		switch (format)
		{
			case EFormat::Color: return IJsonWidget::tr("color");
			case EFormat::Date: return IJsonWidget::tr("date");
			case EFormat::String: return IJsonWidget::tr("string");
		}
		return "";
	}

	//--------------------------------------------------------------------------
	std::optional<EFormat> toEFormat(QString s)
	{
		if (s == json_keys::format_color) { return EFormat::Color; }
		else if (s == json_keys::format_date) { return EFormat::Date; }
		else if (s == json_keys::type_string) { return EFormat::String; }
		else { return std::nullopt; }
	}

	//--------------------------------------------------------------------------
	std::unique_ptr<IJsonWidget> makeWidget(EFormat format, QJsonValue schema)
	{
		switch (format)
		{
			default:
			case EFormat::String:
				return std::make_unique<string::LineEditWidget>(schema);
			case EFormat::Date:
			{
				return std::make_unique<DateStyleSelectorWidget>(schema);
			}
			case EFormat::Color:
				return std::make_unique<ColorDialogButtonWidget>(schema);
		}
	}
} // namespace

//------------------------------------------------------------------------------
StringFormatSelectorWidget::StringFormatSelectorWidget(QJsonValue schema) :
	schema(schema)
{
	formatComboBox.addItem("", static_cast<int>(EFormat::String));
	formatComboBox.addItem("", static_cast<int>(EFormat::Color));
	formatComboBox.addItem("", static_cast<int>(EFormat::Date));

	const auto format = toEFormat(schema[json_keys::key_format].toString());
	constexpr auto defaultFormat = EFormat::String;
	widget = makeWidget(format.value_or(defaultFormat), schema);
	QObject::connect(
		widget.get(), &IJsonWidget::hasChanged, this, &IJsonWidget::hasChanged);

	formatComboBox.setCurrentIndex(formatComboBox.findData(
		static_cast<int>(format.value_or(defaultFormat))));
	if (format)
	{
		formatComboBox.hide();
		formatLabel.hide();
	}

	hLayout.addWidget(&formatLabel);
	hLayout.addWidget(&formatComboBox);

	vLayout.addLayout(&hLayout);
	vLayout.addWidget(widget.get());

	QObject::connect(
		&formatComboBox,
		static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
		this,
		&StringFormatSelectorWidget::replaceWidget);

	setLayout(&vLayout);
	retranslateUi();
}

//------------------------------------------------------------------------------
void StringFormatSelectorWidget::changeEvent(QEvent* event) /* override */
{
	if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
	QWidget::changeEvent(event);
}

//------------------------------------------------------------------------------
void StringFormatSelectorWidget::retranslateUi()
{
	formatLabel.setText(IJsonWidget::tr("format:"));
	for (int i = 0; i != formatComboBox.count(); ++i)
	{
		formatComboBox.setItemText(
			i, ::tr(static_cast<EFormat>(formatComboBox.itemData(i).toInt())));
	}
}

//------------------------------------------------------------------------------
void StringFormatSelectorWidget::replaceWidget()
{
	const auto format = static_cast<EFormat>(
		formatComboBox.itemData(formatComboBox.currentIndex()).toInt());

	auto newWidget = makeWidget(format, schema);
	newWidget->fromQJson(widget->toQJson());
	vLayout.replaceWidget(widget.get(), newWidget.get());
	widget = std::move(newWidget);
	QObject::connect(
		widget.get(), &IJsonWidget::hasChanged, this, &IJsonWidget::hasChanged);
}
