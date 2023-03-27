#include "EditorDialog.h"

#include "ui_EditorDialog.h"
#include "widgets/IJsonWidget.h"

#include <QJsonDocument>
#include <valijson/adapters/qtjson_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

namespace
{

#if 1
	const char* schema = R"({
  "title": "Person",
  "type": "object",
  "required": [
    "name",
    "age",
    "date",
    "favorite_color",
    "gender",
    "location",
    "pets"
  ],
  "properties": {
    "name": {
      "propertyOrder": 1,
      "type": "string",
      "description": "First and Last name",
      "minLength": 4,
      "default": "Jeremy Dorn",
      "placeholder": "John Doe"
    },
    "age": {
      "propertyOrder": 2,
      "type": "integer",
      "default": 25,
      "placeholder": "in years",
      "minimum": 18,
      "maximum": 99
    },
    "favorite_color": {
      "propertyOrder": 3,
      "type": "string",
      "format": "color",
      "title": "favorite color",
      "default": "#ffa500"
    },
    "gender": {
      "propertyOrder": 4,
      "type": "string",
      "enum": [
        "male",
        "female",
        "other"
      ]
    },
    "date": {
      "propertyOrder": 5,
      "type": "string",
      "format": "date",
      "options": {
        "flatpickr": {}
      }
    },
    "location": {
      "propertyOrder": 6,
      "type": "object",
      "title": "Location",
      "properties": {
        "city": {
          "propertyOrder": 1,
          "type": "string",
          "default": "San Francisco"
        },
        "state": {
          "propertyOrder": 2,
          "type": "string",
          "default": "CA"
        },
        "citystate": {
          "propertyOrder": 3,
          "type": "string",
          "description": "This is generated automatically from the previous two fields",
          "template": "{{city}}, {{state}}",
          "watch": {
            "city": "location.city",
            "state": "location.state"
          }
        }
      }
    },
    "pets": {
      "propertyOrder": 7,
      "type": "array",
      "format": "table",
      "title": "Pets",
      "uniqueItems": true,
      "items": {
        "type": "object",
        "title": "Pet",
        "properties": {
          "type": {
            "type": "string",
            "enum": [
              "cat",
              "dog",
              "bird",
              "reptile",
              "other"
            ],
            "default": "dog"
          },
          "name": {
            "type": "string"
          }
        }
      },
      "default": [
        {
          "type": "dog",
          "name": "Walter"
        }
      ]
    }
  }
})";
#endif

} // namespace

enum class EditorDialog::EState : char
{
	Ready,
	UpdatingWidgets,
	UpdatingWidgetValues,
	UpdatingJson
};

//------------------------------------------------------------------------------
EditorDialog::EditorDialog(QWidget* parent) :
	QDialog(parent), ui(new Ui::EditorDialog), state(EState::Ready)
{
	ui->setupUi(this);

	QObject::connect(ui->textEdit_json,
	                 &QTextEdit::textChanged,
	                 this,
	                 &EditorDialog::updateJsonWidgetValues);
	QObject::connect(ui->textEdit_schema,
	                 &QTextEdit::textChanged,
	                 this,
	                 &EditorDialog::updateJsonWidgets);
	QObject::connect(ui->widget,
	                 &QJsonWidget::hasChanged,
	                 this,
	                 &EditorDialog::updateJsonTextValues);

	ui->textEdit_schema->setText(schema); // trigger above message
	updateJsonTextValues();
}

//------------------------------------------------------------------------------
EditorDialog::~EditorDialog() = default;

//------------------------------------------------------------------------------
void EditorDialog::updateJsonWidgets()
{
	if (state != EState::Ready) { return; }
	state = EState::UpdatingWidgets;

	auto text = ui->textEdit_schema->toPlainText();
	QJsonParseError parseError;
	auto doc = QJsonDocument::fromJson(text.toUtf8(), &parseError);

	if (parseError.error == QJsonParseError::NoError)
	{
		auto json = doc.object();
		ui->widget->setSchema(json);
	}
	state = EState::Ready;
}

//------------------------------------------------------------------------------
void EditorDialog::updateJsonWidgetValues()
{
	if (state != EState::Ready) { return; }

	QJsonParseError parseError;
	auto doc = QJsonDocument::fromJson(
		ui->textEdit_json->toPlainText().toUtf8(), &parseError);
	if (parseError.error != QJsonParseError::NoError) { return; }

	QJsonParseError parseErrorSchema;
	auto schemaDoc = QJsonDocument::fromJson(
		ui->textEdit_schema->toPlainText().toUtf8(), &parseErrorSchema);
	if (parseErrorSchema.error != QJsonParseError::NoError) { return; }

#if 1
	valijson::Schema mySchema;
	valijson::SchemaParser parser;
	valijson::adapters::QtJsonAdapter mySchemaAdapter(schemaDoc.object());
	parser.populateSchema(mySchemaAdapter, mySchema);

	valijson::Validator validator;
	valijson::adapters::QtJsonAdapter myTargetAdapter(doc.object());
	if (!validator.validate(mySchema, myTargetAdapter, nullptr)) { return; }
#endif
	state = EState::UpdatingWidgetValues;
	auto json = doc.object();
	ui->widget->setValue(json);

	state = EState::Ready;
	updateJsonTextValues();
}

//------------------------------------------------------------------------------
void EditorDialog::updateJsonTextValues()
{
	if (state != EState::Ready) { return; }
	state = EState::UpdatingJson;

	auto json = ui->widget->getValue();
	auto doc = QJsonDocument(json.toObject());
	auto plainText = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));

	if (ui->textEdit_json->toPlainText() != plainText)
	{
		ui->textEdit_json->setText(plainText);
	}
	state = EState::Ready;
}
