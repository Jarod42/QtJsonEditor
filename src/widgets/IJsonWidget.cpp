#include "IJsonWidget.h"

#include "widgets/array/GridLayoutWidget.h"
#include "widgets/boolean/CheckBoxWidget.h"
#include "widgets/integer/LineEditWidget.h"
#include "widgets/integer/SpinBoxWidget.h"
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
	DescriptiveWidget(QJsonValue json, QString description)
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
		widget = makeWidget(json, "");

		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);

		layout.addWidget(widget.get());
		setLayout(&layout);
	}

	QJsonValue toQJson() const override { return widget->toQJson(); }
	void fromQJson(QJsonValue json) override
	{
		descriptionLabel.setText(json["description"].toString());
		widget->fromQJson(json);
	}

private:
	QLabel descriptionLabel;
	QVBoxLayout layout;
	std::unique_ptr<IJsonWidget> widget;
};

//------------------------------------------------------------------------------
std::unique_ptr<IJsonWidget> makeWidget(QJsonValue json, QString description)
{
	if (!description.isEmpty())
	{
		return std::make_unique<DescriptiveWidget>(json, description);
	}

	const auto type = json["type"];

	if (type == "string")
	{
		if (json.toObject().contains("enum"))
		{
			return std::make_unique<string::ComboBoxWidget>(json);
		}
		const auto format = json["format"];
		if (format == "color")
		{
			return std::make_unique<ColorDialogButtonWidget>(json);
		}
		else if (format == "date")
		{
			return std::make_unique<CalendarWidget>(json);
			//return std::make_unique<DateEditWidget>(json);
		}
		return std::make_unique<string::LineEditWidget>(json);
	}
	else if (type == "integer")
	{
		//return std::make_unique<integer::SpinBoxWidget>(json);
		return std::make_unique<integer::LineEditWidget>(json);
	}
	else if (type == "number")
	{
		return std::make_unique<number::LineEditWidget>(json);
	}
	else if (type == "boolean")
	{
		return std::make_unique<boolean::CheckBoxWidget>(json);
	}
	else if (type == "null")
	{
		// TODO
		return nullptr;
	}
	else if (type == "array")
	{
		return std::make_unique<array::GridLayoutWidget>(json);
	}
	else if (type == "object")
	{
		return std::make_unique<object::GridLayoutWidget>(json);
	}
	return nullptr;
}
