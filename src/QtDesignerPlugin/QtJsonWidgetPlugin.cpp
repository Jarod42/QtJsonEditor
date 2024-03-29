#include "QtJsonWidgetPlugin.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QtUiPlugin/QDesignerExportWidget>

#include "QtJsonWidget/QJsonWidget.h"

//------------------------------------------------------------------------------
/* explicit */ QtJsonWidgetPlugin::QtJsonWidgetPlugin(QObject* parent) :
	QObject(parent)
{}

//------------------------------------------------------------------------------
bool QtJsonWidgetPlugin::isContainer() const /* override */
{
	return false;
}

//------------------------------------------------------------------------------
bool QtJsonWidgetPlugin::isInitialized() const /* override */
{
	return initialized;
}

//------------------------------------------------------------------------------
QIcon QtJsonWidgetPlugin::icon() const /* override */
{
	return QIcon(":/json-file-icon.png");
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::domXml() const /* override */
{
	return R"(<ui language="c++">
    <widget class="QJsonWidget" name="qjsonwidget">
      <property name="schema">
        <string notr="true"></string>
      </property>
      <property name="value">
        <string notr="true">{}</string>
      </property>
    </widget>
</ui>)";
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::group() const /* override */
{
	return QStringLiteral("Input Widgets");
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::includeFile() const /* override */
{
	return QStringLiteral("QtJsonWidget/QJsonWidget.h");
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::name() const /* override */
{
	
	return QStringLiteral("QJsonWidget");
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::toolTip() const /* override */
{
	return "Widget to edit json";
}

//------------------------------------------------------------------------------
QString QtJsonWidgetPlugin::whatsThis() const /* override */
{
	return "Widget to edit json";
}

//------------------------------------------------------------------------------
QWidget* QtJsonWidgetPlugin::createWidget(QWidget* parent) /* override */
{
	auto widget = new QJsonWidget{parent};
	widget->setSchema({});
	return widget;
}

//------------------------------------------------------------------------------
void QtJsonWidgetPlugin::initialize(
	QDesignerFormEditorInterface*) /* override */
{
	if (initialized) { return; }

	initialized = true;
}
