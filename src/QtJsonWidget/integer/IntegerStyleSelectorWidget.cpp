#include "IntegerStyleSelectorWidget.h"

#include "QtJsonWidget/integer/IntegerLineEditWidget.h"
#include "QtJsonWidget/integer/SpinBoxWidget.h"
#include "QtJsonWidget/jsonKeys.h"

namespace integer
{

	namespace
	{

		//----------------------------------------------------------------------
		enum class EStyle
		{
			LineEdit,
			Spinbox
		};

		//----------------------------------------------------------------------
		QString tr(EStyle style)
		{
			switch (style)
			{
				case EStyle::LineEdit: return IJsonWidget::tr("Line edit");
				case EStyle::Spinbox: return IJsonWidget::tr("Spinbox");
			}
			return "";
		}

		//----------------------------------------------------------------------
		std::optional<EStyle> toEStyle(QString s)
		{
			if (s == json_keys::style_lineedit) { return EStyle::LineEdit; }
			else if (s == json_keys::style_spinbox) { return EStyle::Spinbox; }
			else { return std::nullopt; }
		}

		//----------------------------------------------------------------------
		std::unique_ptr<IJsonWidget> makeWidget(EStyle style, QJsonValue schema)
		{
			switch (style)
			{
				default:
				case EStyle::LineEdit:
					return std::make_unique<LineEditWidget>(schema);
				case EStyle::Spinbox:
					return std::make_unique<SpinBoxWidget>(schema);
			}
		}
	} // namespace

	//--------------------------------------------------------------------------
	IntegerStyleSelectorWidget::IntegerStyleSelectorWidget(QJsonValue schema) :
		schema(schema)
	{
		styleComboBox.addItem("", static_cast<int>(EStyle::LineEdit));
		styleComboBox.addItem("", static_cast<int>(EStyle::Spinbox));

		const auto style = toEStyle(schema[json_keys::key_style].toString());
		constexpr auto defaultStyle = EStyle::LineEdit;
		widget = makeWidget(style.value_or(defaultStyle), schema);
		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
		styleComboBox.setCurrentIndex(styleComboBox.findData(
			static_cast<int>(style.value_or(defaultStyle))));
		if (style)
		{
			styleComboBox.hide();
			styleLabel.hide();
		}

		hLayout.addWidget(&styleLabel);
		hLayout.addWidget(&styleComboBox);

		vLayout.addLayout(&hLayout);
		vLayout.addWidget(widget.get());

		QObject::connect(&styleComboBox,
		                 static_cast<void (QComboBox::*)(int)>(
							 &QComboBox::currentIndexChanged),
		                 this,
		                 &IntegerStyleSelectorWidget::replaceWidget);

		setLayout(&vLayout);
		retranslateUi();
	}

	//--------------------------------------------------------------------------
	void IntegerStyleSelectorWidget::changeEvent(QEvent* event) /* override */
	{
		if (event->type() == QEvent::LanguageChange) { retranslateUi(); }
		QWidget::changeEvent(event);
	}

	//--------------------------------------------------------------------------
	void IntegerStyleSelectorWidget::retranslateUi()
	{
		styleLabel.setText(IJsonWidget::tr("style:"));
		for (int i = 0; i != styleComboBox.count(); ++i)
		{
			styleComboBox.setItemText(i,
			                          integer::tr(static_cast<EStyle>(
										  styleComboBox.itemData(i).toInt())));
		}
	}

	//--------------------------------------------------------------------------
	void IntegerStyleSelectorWidget::replaceWidget()
	{
		const auto style = static_cast<EStyle>(
			styleComboBox.itemData(styleComboBox.currentIndex()).toInt());

		auto newWidget = makeWidget(style, schema);
		newWidget->fromQJson(widget->toQJson());
		vLayout.replaceWidget(widget.get(), newWidget.get());
		widget = std::move(newWidget);
		QObject::connect(widget.get(),
		                 &IJsonWidget::hasChanged,
		                 this,
		                 &IJsonWidget::hasChanged);
	}

} // namespace integer
