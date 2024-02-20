#include "EditorDialog.h"

#include "ui_EditorDialog.h"
#include "QtJsonWidget/IJsonWidget.h"

#include <QApplication>
#include <QFileDialog>
#include <QJsonDocument>
#include <QLibraryInfo>
#include <QTranslator>
#include <array>
#include <valijson/adapters/qtjson_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

namespace
{

#if 1

	const char* schema7 = R"(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "$id": "http://json-schema.org/draft-07/schema#",
  "title": "Core schema meta-schema",
  "definitions": {
    "schemaArray": {
      "type": "array",
      "minItems": 1,
      "items": {"$ref": "#"}
    },
    "nonNegativeInteger": {
      "type": "integer",
      "minimum": 0
    },
    "nonNegativeIntegerDefault0": {
      "allOf": [{"$ref": "#/definitions/nonNegativeInteger"}, {"default": 0}]
    },
    "simpleTypes": {
      "enum": ["array", "boolean", "integer", "null", "number", "object", "string"]
    },
    "stringArray": {
      "type": "array",
      "items": {"type": "string"},
      "uniqueItems": true,
      "default": []
    }
  },
  "type": ["object", "boolean"],
  "properties": {
    "$id": {
      "type": "string",
      "format": "uri-reference"
    },
    "$schema": {
      "type": "string",
      "format": "uri"
    },
    "$ref": {
      "type": "string",
      "format": "uri-reference"
    },
    "$comment": {
      "type": "string"
    },
    "title": {
      "type": "string"
    },
    "description": {
      "type": "string"
    },
    "default": true,
    "readOnly": {
      "type": "boolean",
      "default": false
    },
    "examples": {
      "type": "array",
      "items": true
    },
    "multipleOf": {
      "type": "number",
      "exclusiveMinimum": 0
    },
    "maximum": {
      "type": "number"
    },
    "exclusiveMaximum": {
      "type": "number"
    },
    "minimum": {
      "type": "number"
    },
    "exclusiveMinimum": {
      "type": "number"
    },
    "maxLength": {"$ref": "#/definitions/nonNegativeInteger"},
    "minLength": {"$ref": "#/definitions/nonNegativeIntegerDefault0"},
    "pattern": {
      "type": "string",
      "format": "regex"
    },
    "additionalItems": {"$ref": "#"},
    "items": {
      "anyOf": [{"$ref": "#"}, {"$ref": "#/definitions/schemaArray"}],
      "default": true
    },
    "maxItems": {"$ref": "#/definitions/nonNegativeInteger"},
    "minItems": {"$ref": "#/definitions/nonNegativeIntegerDefault0"},
    "uniqueItems": {
      "type": "boolean",
      "default": false
    },
    "contains": {"$ref": "#"},
    "maxProperties": {"$ref": "#/definitions/nonNegativeInteger"},
    "minProperties": {"$ref": "#/definitions/nonNegativeIntegerDefault0"},
    "required": {"$ref": "#/definitions/stringArray"},
    "additionalProperties": {"$ref": "#"},
    "definitions": {
      "type": "object",
      "additionalProperties": {"$ref": "#"},
      "default": {}
    },
    "properties": {
      "type": "object",
      "additionalProperties": {"$ref": "#"},
      "default": {}
    },
    "patternProperties": {
      "type": "object",
      "additionalProperties": {"$ref": "#"},
      "propertyNames": {"format": "regex"},
      "default": {}
    },
    "dependencies": {
      "type": "object",
      "additionalProperties": {
        "anyOf": [{"$ref": "#"}, {"$ref": "#/definitions/stringArray"}]
      }
    },
    "propertyNames": {"$ref": "#"},
    "const": true,
    "enum": {
      "type": "array",
      "items": true,
      "minItems": 1,
      "uniqueItems": true
    },
    "type": {
      "anyOf": [
        {"$ref": "#/definitions/simpleTypes"},
        {
          "type": "array",
          "items": {"$ref": "#/definitions/simpleTypes"},
          "minItems": 1,
          "uniqueItems": true
        }
      ]
    },
    "format": {"type": "string"},
    "contentMediaType": {"type": "string"},
    "contentEncoding": {"type": "string"},
    "if": {"$ref": "#"},
    "then": {"$ref": "#"},
    "else": {"$ref": "#"},
    "allOf": {"$ref": "#/definitions/schemaArray"},
    "anyOf": {"$ref": "#/definitions/schemaArray"},
    "oneOf": {"$ref": "#/definitions/schemaArray"},
    "not": {"$ref": "#"}
  },
  "default": true
}
)";

	const char* initialSchema = R"({
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

	//--------------------------------------------------------------------------
	bool isValid(const QString& schema, const QString& json)
	{
		QJsonParseError parseError;
		auto schemaDoc = QJsonDocument::fromJson(schema.toUtf8(), &parseError);
		if (parseError.error != QJsonParseError::NoError) { return false; }

		QJsonParseError parseErrorSchema;
		auto jsonDoc =
			QJsonDocument::fromJson(json.toUtf8(), &parseErrorSchema);
		if (parseErrorSchema.error != QJsonParseError::NoError)
		{
			return false;
		}

		valijson::Schema mySchema;
		valijson::SchemaParser parser;
		valijson::adapters::QtJsonAdapter qtSchemaAdapter(schemaDoc.object());
		parser.populateSchema(qtSchemaAdapter, mySchema);

		valijson::Validator validator;
		valijson::adapters::QtJsonAdapter myTargetAdapter(jsonDoc.object());
		return validator.validate(mySchema, myTargetAdapter, nullptr);
	}

} // namespace

enum class EditorDialog::EState : char
{
	Ready,
	UpdatingWidgets,
	UpdatingWidgetValues,
	UpdatingJson
};

//------------------------------------------------------------------------------
EditorDialog::EditorDialog(QTranslator& qtTranslator,
                           QTranslator& translator,
                           QWidget* parent) :
	QMainWindow(parent),
	ui(std::make_unique<Ui::EditorDialog>()), qtTranslator(qtTranslator),
	translator(translator), state(EState::Ready)
{
	ui->setupUi(this);

	QObject::connect(ui->actionOpenSchema, &QAction::triggered, this, [this]() {
		auto filename = QFileDialog::getOpenFileName(
			this,
			IJsonWidget::tr("Select Schema"),
			"",
			IJsonWidget::tr("Json Files (*.json);;All files (*.*)"));

		if (filename.isEmpty()) { return; }
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return; }
		ui->textEdit_schema->setText(QString::fromUtf8(file.readAll()));
	});
	QObject::connect(ui->actionOpenJson, &QAction::triggered, this, [this]() {
		auto filename = QFileDialog::getOpenFileName(
			this,
			IJsonWidget::tr("Select Json"),
			"",
			IJsonWidget::tr("Json Files (*.json);;All files (*.*)"));

		if (filename.isEmpty()) { return; }
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { return; }
		ui->textEdit_json->setText(QString::fromUtf8(file.readAll()));
	});
	QObject::connect(ui->actionSaveSchema, &QAction::triggered, this, [this]() {
		auto filename = QFileDialog::getSaveFileName(
			this,
			IJsonWidget::tr("Select Schema"),
			"",
			IJsonWidget::tr("Json Files (*.json);;All files (*.*)"));

		if (filename.isEmpty()) { return; }
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
		file.write(ui->textEdit_schema->toPlainText().toUtf8());
	});
	QObject::connect(ui->actionSaveJson, &QAction::triggered, this, [this]() {
		auto filename = QFileDialog::getSaveFileName(
			this,
			IJsonWidget::tr("Select Json"),
			"",
			IJsonWidget::tr("Json Files (*.json);;All files (*.*)"));

		if (filename.isEmpty()) { return; }
		QFile file(filename);
		if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return; }
		file.write(ui->textEdit_json->toPlainText().toUtf8());
	});

	QObject::connect(ui->actionEnglish, &QAction::triggered, this, [this]() {
		ui->actionFrench->setChecked(false);
		ui->actionEnglish->setChecked(true);
		this->qtTranslator.load(
			"qt_en", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		this->translator.load("qtjsoneditor_en.qm",
		                      QApplication::applicationDirPath());
		QLocale::setDefault(QLocale(QLocale::English));
	});
	QObject::connect(ui->actionFrench, &QAction::triggered, this, [this]() {
		ui->actionFrench->setChecked(true);
		ui->actionEnglish->setChecked(false);
		this->qtTranslator.load(
			"qt_fr", QLibraryInfo::location(QLibraryInfo::TranslationsPath));
		this->translator.load("qtjsoneditor_fr.qm",
		                      QApplication::applicationDirPath());
		QLocale::setDefault(QLocale(QLocale::French));
	});
	QObject::connect(ui->textEdit_json,
	                 &QTextEdit::textChanged,
	                 this,
	                 &EditorDialog::onJsonTextChanged);
	QObject::connect(ui->pushButton_updateJsonValue,
	                 &QPushButton::clicked,
	                 this,
	                 &EditorDialog::updateJsonWidgetValues);
	QObject::connect(ui->textEdit_schema,
	                 &QTextEdit::textChanged,
	                 this,
	                 &EditorDialog::onSchemaChanged);
	QObject::connect(ui->pushButton_updateSchema,
	                 &QPushButton::clicked,
	                 this,
	                 &EditorDialog::updateJsonWidgets);
	QObject::connect(ui->widget,
	                 &QJsonWidget::valueChanged,
	                 this,
	                 &EditorDialog::updateJsonTextValues);

	ui->textEdit_schema->setText(initialSchema); // trigger above message
	if (ui->pushButton_updateSchema->isEnabled()) { updateJsonWidgets(); }
	updateJsonTextValues();
}

//------------------------------------------------------------------------------
EditorDialog::~EditorDialog() = default;

//------------------------------------------------------------------------------
void EditorDialog::changeEvent(QEvent* event) /* override */
{
	if (event->type() == QEvent::LanguageChange) { ui->retranslateUi(this); }
	QWidget::changeEvent(event);
}

//------------------------------------------------------------------------------
void EditorDialog::onSchemaChanged()
{
	ui->pushButton_updateSchema->setEnabled(
		isValid(schema7, ui->textEdit_schema->toPlainText()));
}

//------------------------------------------------------------------------------
void EditorDialog::onJsonTextChanged()
{
	ui->pushButton_updateJsonValue->setEnabled(isValid(
		ui->textEdit_schema->toPlainText(), ui->textEdit_json->toPlainText()));
}

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
