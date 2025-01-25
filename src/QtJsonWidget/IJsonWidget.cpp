#include "IJsonWidget.h"

#include "QtJsonWidget/JsonReferenceResolver.h"
#include "QtJsonWidget/array/ArrayGridLayoutWidget.h"
#include "QtJsonWidget/boolean/CheckBoxWidget.h"
#include "QtJsonWidget/integer/IntegerStyleSelectorWidget.h"
#include "QtJsonWidget/jsonKeys.h"
#include "QtJsonWidget/null/LabelWidget.h"
#include "QtJsonWidget/number/NumberLineEditWidget.h"
#include "QtJsonWidget/object/ObjectGridLayoutWidget.h"
#include "QtJsonWidget/string/StringFormatSelectorWidget.h"
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

		widget = std::make_unique<null::LabelWidget>();
		layout.addWidget(widget.get());
		if (typeComboBox.count() != 0)
		{
			typeComboBox.setCurrentIndex(0);
			typeCurrentIndexChanged();
		}
		QObject::connect(&typeComboBox,
		                 static_cast<void (QComboBox::*)(int index)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &TypeSelectorWidget::typeCurrentIndexChanged);

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

		for (int i = 0; i != typeComboBox.count(); ++i)
		{
			typeComboBox.setItemText(
				i, tr(static_cast<EType>(typeComboBox.itemData(i).toInt())));
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
			typeComboBox.addItem("", static_cast<int>(EType::Null));
		}
		if (hasAny || types.contains(json_keys::type_boolean))
		{
			typeComboBox.addItem("", static_cast<int>(EType::Boolean));
		}
		if (hasAny || types.contains(json_keys::type_integer))
		{
			typeComboBox.addItem("", static_cast<int>(EType::Integer));
		}
		if (hasAny || types.contains(json_keys::type_number))
		{
			typeComboBox.addItem("", static_cast<int>(EType::Double));
		}
		if (hasAny || types.contains(json_keys::type_string))
		{
			typeComboBox.addItem("", static_cast<int>(EType::String));
		}
		if (hasAny || types.contains(json_keys::type_array))
		{
			typeComboBox.addItem("", static_cast<int>(EType::Array));
		}
		if (hasAny || types.contains(json_keys::type_object))
		{
			typeComboBox.addItem("", static_cast<int>(EType::Object));
		}
		// types.contains({"$ref": "#"})
	}

	//--------------------------------------------------------------------------
	void replaceWidget()
	{
		const auto type = static_cast<EType>(
			typeComboBox.itemData(typeComboBox.currentIndex()).toInt());

		auto newWidget = makeWidget(type);
		layout.replaceWidget(widget.get(), newWidget.get());
		widget = std::move(newWidget);
		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
		emit hasChanged();
	}

	//--------------------------------------------------------------------------
	void typeCurrentIndexChanged()
	{
		replaceWidget();
	}

	//--------------------------------------------------------------------------
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
				return std::make_unique<integer::IntegerStyleSelectorWidget>(
					schema);
			case EType::String:
			{
				return std::make_unique<StringFormatSelectorWidget>(schema);
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
	QLabel typeLabel;
	QComboBox typeComboBox;
	std::unique_ptr<IJsonWidget> widget;
};

//------------------------------------------------------------------------------
class LabelWidget : public IJsonWidget
{
public:
	explicit LabelWidget(QString text)
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
		return std::make_unique<StringFormatSelectorWidget>(json);
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
