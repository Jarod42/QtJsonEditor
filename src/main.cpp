#include "ui/EditorDialog.h"

#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	EditorDialog dialog;
	dialog.show();
	return app.exec();
}
