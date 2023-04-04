#include "IJsonWidget.h"

#include "widgets/JsonReferenceResolver.h"
#include "widgets/array/GridLayoutWidget.h"
#include "widgets/boolean/CheckBoxWidget.h"
#include "widgets/integer/LineEditWidget.h"
#include "widgets/integer/SpinBoxWidget.h"
#include "widgets/jsonKeys.h"
#include "widgets/null/LabelWidget.h"
#include "widgets/number/LineEditWidget.h"
#include "widgets/object/GridLayoutWidget.h"
#include "widgets/string/LineEditWidget.h"
#include "widgets/string/color/ColorDialogButtonWidget.h"
#include "widgets/string/date/CalendarWidget.h"
#include "widgets/string/date/DateEditWidget.h"
#include "widgets/string/enum/ComboBoxWidget.h"

#include <QJsonObject>
#include <QLabel>

class DescriptiveWidget : public IJsonWidget
{
public:
	DescriptiveWidget(const JsonReferenceResolver& jsonReferenceResolver,
	                  QJsonValue json,
	                  QString description)
	{
		descriptionLabel.setObjectName(
			get_unique_name("(DescriptiveWidget)label-"));
		layout.setObjectName(get_unique_name("(DescriptiveWidget)layout-"));

		auto smallerFont = descriptionLabel.font();
		smallerFont.setPointSize(smallerFont.pointSize() - 2);
		smallerFont.setItalic(true);
		descriptionLabel.setFont(smallerFont);
		descriptionLabel.setText(description);
		layout.addWidget(&descriptionLabel);
		widget = makeWidget(jsonReferenceResolver, json, EDescription::Without);

		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);

		layout.addWidget(widget.get());
		setLayout(&layout);
	}

	QJsonValue toQJson() const override { return widget->toQJson(); }
	void fromQJson(QJsonValue json) override { widget->fromQJson(json); }

private:
	QLabel descriptionLabel;
	QVBoxLayout layout;
	std::unique_ptr<IJsonWidget> widget;
};

//------------------------------------------------------------------------------
class LabelWidget : public IJsonWidget
{
public:
	LabelWidget(QString text)
	{
		label.setObjectName(get_unique_name("(LabelWidget)label-"));
		layout.setObjectName(get_unique_name("(LabelWidget)layout-"));

		label.setText(text);
		layout.addWidget(&label);

		setLayout(&layout);
	}

	QJsonValue toQJson() const override { return {}; }
	void fromQJson(QJsonValue) override {}

private:
	QLabel label;
	QVBoxLayout layout;
};

//------------------------------------------------------------------------------
std::unique_ptr<IJsonWidget>
makeWidget(const JsonReferenceResolver& jsonReferenceResolver,
           QJsonValue json,
           EDescription descriptionMode)
{
	auto ref = json[json_keys::key_ref].toString();
	if (!ref.isEmpty())
	{
		json = jsonReferenceResolver.resolveReference(ref, json);
	}
	if (descriptionMode == EDescription::With)
	{
		QString description = json[json_keys::key_description].toString();
		if (!description.isEmpty())
		{
			return std::make_unique<DescriptiveWidget>(
				jsonReferenceResolver, json, description);
		}
	}

	const auto type = json[json_keys::key_type];

	if (type == json_keys::type_string)
	{
		if (json.toObject().contains(json_keys::key_enum))
		{
			return std::make_unique<string::ComboBoxWidget>(json);
		}
		const auto format = json[json_keys::key_format];
		if (format == json_keys::format_color)
		{
			return std::make_unique<ColorDialogButtonWidget>(json);
		}
		else if (format == json_keys::format_date)
		{
			return std::make_unique<CalendarWidget>(json);
			//return std::make_unique<DateEditWidget>(json);
		}
		return std::make_unique<string::LineEditWidget>(json);
	}
	else if (type == json_keys::type_integer)
	{
		//return std::make_unique<integer::SpinBoxWidget>(json);
		return std::make_unique<integer::LineEditWidget>(json);
	}
	else if (type == json_keys::type_number)
	{
		return std::make_unique<number::LineEditWidget>(json);
	}
	else if (type == json_keys::type_boolean)
	{
		return std::make_unique<boolean::CheckBoxWidget>(json);
	}
	else if (type == json_keys::type_null)
	{
		return std::make_unique<null::LabelWidget>();
	}
	else if (type == json_keys::type_array)
	{
		return std::make_unique<array::GridLayoutWidget>(jsonReferenceResolver,
		                                                 json);
	}
	else if (type == json_keys::type_object)
	{
		return std::make_unique<object::GridLayoutWidget>(jsonReferenceResolver,
		                                                  json);
	}
	return std::make_unique<LabelWidget>("Not supported type (or missing)");
}
