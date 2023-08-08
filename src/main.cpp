#include "ui/EditorDialog.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QTranslator translator;
	if (!translator.load("qtjsoneditor_en.qm",
	                     QCoreApplication::applicationDirPath()))
	{
		return -1;
	}
	app.installTranslator(&translator);

	EditorDialog dialog(translator);
	dialog.show();
	return app.exec();
}
