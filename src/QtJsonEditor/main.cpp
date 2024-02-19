#include "QtJsonEditorDialog/EditorDialog.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QLocale::setDefault(QLocale::English);

	QTranslator qtTranslator;
	qtTranslator.load("qt_en",
	                  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	app.installTranslator(&qtTranslator);

	QTranslator translator;
	if (!translator.load("qtjsoneditor_en.qm",
	                     QCoreApplication::applicationDirPath()))
	{
		return -1;
	}
	app.installTranslator(&translator);

	EditorDialog dialog(qtTranslator, translator);
	dialog.show();
	return app.exec();
}
