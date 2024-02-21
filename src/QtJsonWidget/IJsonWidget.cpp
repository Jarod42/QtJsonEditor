#include "IJsonWidget.h"

#include "QtJsonWidget/JsonReferenceResolver.h"
#include "QtJsonWidget/array/ArrayGridLayoutWidget.h"
#include "QtJsonWidget/boolean/CheckBoxWidget.h"
#include "QtJsonWidget/integer/IntegerLineEditWidget.h"
#include "QtJsonWidget/integer/SpinBoxWidget.h"
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

public:
	TypeSelectorWidget(const JsonReferenceResolver& jsonReferenceResolver,
	                   QJsonValue json) :
		jsonReferenceResolver(jsonReferenceResolver),
		schema(json)
	{
		label.setObjectName(get_unique_name("(TypeSelectorWidget)label-"));
		typeLayout.setObjectName(
			get_unique_name("(TypeSelectorWidget)typeLayout-"));

		typeLayout.addWidget(&label);
		fillComboBox(json[json_keys::key_type]);
		typeLayout.addWidget(&comboBox);
		layout.addLayout(&typeLayout);

		widget = std::make_unique<null::LabelWidget>();
		layout.addWidget(widget.get());
		if (comboBox.count() != 0)
		{
			comboBox.setCurrentIndex(0);
			currentIndexChanged(0);
		}
		QObject::connect(&comboBox,
		                 static_cast<void (QComboBox::*)(int index)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &TypeSelectorWidget::currentIndexChanged);

		setLayout(&layout);
		retranslateUi();
	}

	QJsonValue toQJson() const override { return widget->toQJson(); }
	void fromQJson(QJsonValue json) override
	{
		auto type = deduceEType(json);
		if (comboBox.currentData().toInt() != static_cast<int>(type))
		{
			comboBox.setCurrentIndex(comboBox.findData(static_cast<int>(type)));
		}
		widget->fromQJson(json);
	}

	void changeEvent(QEvent* event) override
	{
		if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
		QWidget::changeEvent(event);
	}

	void retranslateUi()
	{
		label.setText(IJsonWidget::tr("type:"));

		for (int i = 0; i != comboBox.count(); ++i)
		{
			switch (EType(comboBox.itemData(i).toInt()))
			{
				case EType::Null:
					comboBox.setItemText(i, IJsonWidget::tr("null"));
					break;
				case EType::Boolean:
					comboBox.setItemText(i, IJsonWidget::tr("boolean"));
					break;
				case EType::Integer:
					comboBox.setItemText(i, IJsonWidget::tr("integer"));
					break;
				case EType::Double:
					comboBox.setItemText(i, IJsonWidget::tr("number"));
					break;
				case EType::String:
					comboBox.setItemText(i, IJsonWidget::tr("string"));
					break;
				case EType::Array:
					comboBox.setItemText(i, IJsonWidget::tr("array"));
					break;
				case EType::Object:
					comboBox.setItemText(i, IJsonWidget::tr("object"));
					break;
			}
		}
	}

private:
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

	void fillComboBox(QJsonValue type)
	{
		const auto types = type.toArray();
		const bool hasAny = types.contains(json_keys::type_any)
		                 || type.toString() == json_keys::type_any
		                 || type.isUndefined();
		if (hasAny || types.contains(json_keys::type_null))
		{
			comboBox.addItem("", int(EType::Null));
		}
		if (hasAny || types.contains(json_keys::type_boolean))
		{
			comboBox.addItem("", int(EType::Boolean));
		}
		if (hasAny || types.contains(json_keys::type_integer))
		{
			comboBox.addItem("", int(EType::Integer));
		}
		if (hasAny || types.contains(json_keys::type_number))
		{
			comboBox.addItem("", int(EType::Double));
		}
		if (hasAny || types.contains(json_keys::type_string))
		{
			comboBox.addItem("", int(EType::String));
		}
		if (hasAny || types.contains(json_keys::type_array))
		{
			comboBox.addItem("", int(EType::Array));
		}
		if (hasAny || types.contains(json_keys::type_object))
		{
			comboBox.addItem("", int(EType::Object));
		}
		// types.contains({"$ref": "#"})
	}

	void currentIndexChanged(int index)
	{
		auto newWidget =
			makeWidget(static_cast<EType>(comboBox.itemData(index).toInt()));
		layout.replaceWidget(widget.get(), newWidget.get());
		widget = std::move(newWidget);
		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
		emit hasChanged();
	}

	std::unique_ptr<IJsonWidget> makeWidget(EType type)
	{
		switch (type)
		{
			case EType::Null: return std::make_unique<null::LabelWidget>();
			case EType::Boolean:
				return std::make_unique<boolean::CheckBoxWidget>(schema);
			case EType::Double:
				return std::make_unique<number::LineEditWidget>(schema);
			case EType::Integer:
				return std::make_unique<integer::LineEditWidget>(schema);
			case EType::String:
				return std::make_unique<string::LineEditWidget>(schema);
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
	QLabel label;
	QComboBox comboBox;
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
	return std::make_unique<TypeSelectorWidget>(jsonReferenceResolver, json);
}
