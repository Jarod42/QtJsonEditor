#include "IJsonWidget.h"

#include "QtJsonWidget/JsonReferenceResolver.h"
#include "QtJsonWidget/array/ArrayGridLayoutWidget.h"
#include "QtJsonWidget/boolean/CheckBoxWidget.h"
#include "QtJsonWidget/integer/IntegerStyleSelectorWidget.h"
#include "QtJsonWidget/jsonKeys.h"
#include "QtJsonWidget/null/LabelWidget.h"
#include "QtJsonWidget/number/NumberLineEditWidget.h"
#include "QtJsonWidget/object/ObjectGridLayoutWidget.h"
#include "QtJsonWidget/string/StringLineEditWidget.h"
#include "QtJsonWidget/string/color/ColorDialogButtonWidget.h"
#include "QtJsonWidget/string/date/CalendarWidget.h"
#include "QtJsonWidget/string/date/DateEditWidget.h"
#include "QtJsonWidget/string/enum/ComboBoxWidget.h"

#include <QEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>

//------------------------------------------------------------------------------
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
	QVBoxLayout layout;
	QLabel descriptionLabel;
	std::unique_ptr<IJsonWidget> widget;
};

//------------------------------------------------------------------------------
class TypeSelectorWidget : public IJsonWidget
{
	enum class EType
	{
		Null,
		Boolean,
		Integer,
		Double,
		String,
		Array,
		Object
	};

	//--------------------------------------------------------------------------
	static QString tr(EType type)
	{
		switch (type)
		{
			case EType::Null: return IJsonWidget::tr("null");
			case EType::Boolean: return IJsonWidget::tr("boolean");
			case EType::Integer: return IJsonWidget::tr("integer");
			case EType::Double: return IJsonWidget::tr("number");
			case EType::String: return IJsonWidget::tr("string");
			case EType::Array: return IJsonWidget::tr("array");
			case EType::Object: return IJsonWidget::tr("object");
		}
		return "";
	}

	enum class EStringFormat
	{
		String,
		Color,
		Date
	};

	//--------------------------------------------------------------------------
	static QString tr(EStringFormat stringFormat)
	{
		switch (stringFormat)
		{
			case EStringFormat::Color: return IJsonWidget::tr("color");
			case EStringFormat::Date: return IJsonWidget::tr("date");
			case EStringFormat::String: return IJsonWidget::tr("string");
		}
		return "";
	}

	enum class EDateStyle
	{
		Calendar,
		DateEdit,
	};

	//--------------------------------------------------------------------------
	static QString tr(EDateStyle style)
	{
		switch (style)
		{
			case EDateStyle::Calendar: return IJsonWidget::tr("Calendar");
			case EDateStyle::DateEdit: return IJsonWidget::tr("Date edit");
		}
		return "";
	}

public:
	//--------------------------------------------------------------------------
	TypeSelectorWidget(const JsonReferenceResolver& jsonReferenceResolver,
	                   QJsonValue json) :
		jsonReferenceResolver(jsonReferenceResolver),
		schema(json)
	{
		typeLabel.setObjectName(get_unique_name("(TypeSelectorWidget)label-"));
		typeLayout.setObjectName(
			get_unique_name("(TypeSelectorWidget)typeLayout-"));

		typeLayout.addWidget(&typeLabel);
		fillTypeComboBox(json[json_keys::key_type]);
		typeLayout.addWidget(&typeComboBox);
		layout.addLayout(&typeLayout);

		formatLayout.addWidget(&formatLabel);
		fillStringFormatComboBox();
		formatLayout.addWidget(&stringFormatComboBox);
		layout.addLayout(&formatLayout);

		styleLayout.addWidget(&styleLabel);
		fillStyleComboBoxes();
		styleLayout.addWidget(&dateStyleComboBox);
		layout.addLayout(&styleLayout);

		widget = std::make_unique<null::LabelWidget>();
		layout.addWidget(widget.get());
		if (typeComboBox.count() != 0)
		{
			typeComboBox.setCurrentIndex(0);
			typeCurrentIndexChanged(0);
		}
		QObject::connect(&typeComboBox,
		                 static_cast<void (QComboBox::*)(int index)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &TypeSelectorWidget::typeCurrentIndexChanged);
		QObject::connect(&stringFormatComboBox,
		                 static_cast<void (QComboBox::*)(int index)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &TypeSelectorWidget::stringFormatCurrentIndexChanged);

		QObject::connect(&dateStyleComboBox,
		                 static_cast<void (QComboBox::*)(int index)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &TypeSelectorWidget::replaceWidget);

		setLayout(&layout);
		retranslateUi();
	}

	//--------------------------------------------------------------------------
	QJsonValue toQJson() const override { return widget->toQJson(); }

	//--------------------------------------------------------------------------
	void fromQJson(QJsonValue json) override
	{
		auto type = deduceEType(json);
		if (typeComboBox.currentData().toInt() != static_cast<int>(type))
		{
			typeComboBox.setCurrentIndex(
				typeComboBox.findData(static_cast<int>(type)));
		}
		widget->fromQJson(json);
	}

	//--------------------------------------------------------------------------
	void changeEvent(QEvent* event) override
	{
		if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
		QWidget::changeEvent(event);
	}

	//--------------------------------------------------------------------------
	void retranslateUi()
	{
		typeLabel.setText(IJsonWidget::tr("type:"));
		formatLabel.setText(IJsonWidget::tr("format:"));
		styleLabel.setText(IJsonWidget::tr("style:"));

		for (int i = 0; i != typeComboBox.count(); ++i)
		{
			typeComboBox.setItemText(
				i, tr(EType(typeComboBox.itemData(i).toInt())));
		}
		for (int i = 0; i != stringFormatComboBox.count(); ++i)
		{
			stringFormatComboBox.setItemText(
				i, tr(EStringFormat(stringFormatComboBox.itemData(i).toInt())));
		}
		for (int i = 0; i != dateStyleComboBox.count(); ++i)
		{
			dateStyleComboBox.setItemText(
				i, tr(EDateStyle(dateStyleComboBox.itemData(i).toInt())));
		}
	}

private:
	//--------------------------------------------------------------------------
	static EType deduceEType(QJsonValue json)
	{
		switch (json.type())
		{
			case QJsonValue::Type::Null: return EType::Null;
			case QJsonValue::Type::Bool: return EType::Boolean;
			case QJsonValue::Type::Double:
				if (json.toInt() == json.toDouble()) { return EType::Integer; }
				else { return EType::Double; }
			case QJsonValue::Type::String: return EType::String;
			case QJsonValue::Type::Array: return EType::Array;
			case QJsonValue::Type::Object: return EType::Object;
			default: return EType::Null;
		}
	}

	//--------------------------------------------------------------------------
	void fillTypeComboBox(QJsonValue type)
	{
		const auto types = type.toArray();
		const bool hasAny = types.contains(json_keys::type_any)
		                 || type.toString() == json_keys::type_any
		                 || type.isUndefined();
		if (hasAny || types.contains(json_keys::type_null))
		{
			typeComboBox.addItem("", int(EType::Null));
		}
		if (hasAny || types.contains(json_keys::type_boolean))
		{
			typeComboBox.addItem("", int(EType::Boolean));
		}
		if (hasAny || types.contains(json_keys::type_integer))
		{
			typeComboBox.addItem("", int(EType::Integer));
		}
		if (hasAny || types.contains(json_keys::type_number))
		{
			typeComboBox.addItem("", int(EType::Double));
		}
		if (hasAny || types.contains(json_keys::type_string))
		{
			typeComboBox.addItem("", int(EType::String));
		}
		if (hasAny || types.contains(json_keys::type_array))
		{
			typeComboBox.addItem("", int(EType::Array));
		}
		if (hasAny || types.contains(json_keys::type_object))
		{
			typeComboBox.addItem("", int(EType::Object));
		}
		// types.contains({"$ref": "#"})
	}

	//--------------------------------------------------------------------------
	void fillStringFormatComboBox()
	{
		stringFormatComboBox.addItem("", int(EStringFormat::String));
		stringFormatComboBox.addItem("", int(EStringFormat::Color));
		stringFormatComboBox.addItem("", int(EStringFormat::Date));
	}

	//--------------------------------------------------------------------------
	void fillStyleComboBoxes()
	{
		dateStyleComboBox.addItem("", int(EDateStyle::Calendar));
		dateStyleComboBox.addItem("", int(EDateStyle::DateEdit));
	}

	//--------------------------------------------------------------------------
	void replaceWidget()
	{
		const auto type = static_cast<EType>(
			typeComboBox.itemData(typeComboBox.currentIndex()).toInt());
		const auto stringFormat = static_cast<EStringFormat>(
			stringFormatComboBox.itemData(stringFormatComboBox.currentIndex())
				.toInt());
		const auto dateStyle = static_cast<EDateStyle>(
			dateStyleComboBox.itemData(dateStyleComboBox.currentIndex())
				.toInt());

		auto newWidget = makeWidget(type, stringFormat, dateStyle);
		layout.replaceWidget(widget.get(), newWidget.get());
		widget = std::move(newWidget);
		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
		emit hasChanged();
	}

	//--------------------------------------------------------------------------
	void typeCurrentIndexChanged(int index)
	{
		const auto type =
			static_cast<EType>(typeComboBox.itemData(index).toInt());
		switch (type)
		{
			case EType::String:
			{
				formatLabel.show();
				stringFormatComboBox.show();
				styleLabel.hide();
				dateStyleComboBox.hide();
				break;
			}
			default:
			{
				formatLabel.hide();
				stringFormatComboBox.hide();
				styleLabel.hide();
				dateStyleComboBox.hide();
				break;
			}
		}
		replaceWidget();
	}

	//--------------------------------------------------------------------------
	void stringFormatCurrentIndexChanged(int index)
	{
		const auto stringFormat = static_cast<EStringFormat>(
			stringFormatComboBox.itemData(index).toInt());

		stringFormatComboBox.show();
		switch (stringFormat)
		{
			default:
			case EStringFormat::Color:
			case EStringFormat::String:
			{
				styleLabel.hide();
				dateStyleComboBox.hide();
				break;
			}
			case EStringFormat::Date:
			{
				styleLabel.show();
				dateStyleComboBox.show();
				break;
			}
		}
		replaceWidget();
	}

	//--------------------------------------------------------------------------
	std::unique_ptr<IJsonWidget>
	makeWidget(EType type, EStringFormat format, EDateStyle dateStyle)
	{
		switch (type)
		{
			case EType::Null: return std::make_unique<null::LabelWidget>();
			case EType::Boolean:
				return std::make_unique<boolean::CheckBoxWidget>(schema);
			case EType::Double:
				return std::make_unique<number::LineEditWidget>(schema);
			case EType::Integer:
				return std::make_unique<integer::IntegerStyleSelectorWidget>(
					schema);
			case EType::String:
			{
				switch (format)
				{
					default:
					case EStringFormat::String:
						return std::make_unique<string::LineEditWidget>(schema);
					case EStringFormat::Date:
					{
						switch (dateStyle)
						{
							default:
							case EDateStyle::Calendar:
								return std::make_unique<CalendarWidget>(schema);
							case EDateStyle::DateEdit:
								return std::make_unique<DateEditWidget>(schema);
						}
					}
					case EStringFormat::Color:
						return std::make_unique<ColorDialogButtonWidget>(
							schema);
				}
			}
			case EType::Array:
				return std::make_unique<array::GridLayoutWidget>(
					jsonReferenceResolver, schema);
			case EType::Object:
				return std::make_unique<object::GridLayoutWidget>(
					jsonReferenceResolver, schema);
		}
		return std::make_unique<null::LabelWidget>();
	}

private:
	const JsonReferenceResolver& jsonReferenceResolver;
	QJsonValue schema;
	QVBoxLayout layout;
	QHBoxLayout typeLayout;
	QHBoxLayout formatLayout;
	QHBoxLayout styleLayout;
	QLabel typeLabel;
	QLabel formatLabel;
	QLabel styleLabel;
	QComboBox typeComboBox;
	QComboBox stringFormatComboBox;
	QComboBox dateStyleComboBox;
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
			const auto style = json[json_keys::key_style];
			if (style == json_keys::style_calendar)
			{
				return std::make_unique<CalendarWidget>(json);
			}
			else if (style == json_keys::style_dateedit)
			{
				return std::make_unique<DateEditWidget>(json);
			}
			else { return std::make_unique<CalendarWidget>(json); }
		}
		return std::make_unique<string::LineEditWidget>(json);
	}
	else if (type == json_keys::type_integer)
	{
		return std::make_unique<integer::IntegerStyleSelectorWidget>(json);
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
	return std::make_unique<TypeSelectorWidget>(jsonReferenceResolver, json);
}
