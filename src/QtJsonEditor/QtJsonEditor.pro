TEMPLATE = app

CONFIG += windows c++17
QT += widgets # core gui defaulted

INCLUDEPATH += ".."

SOURCES += main.cpp

debug:DEFINES += DEBUG
release:DEFINES += NDEBUG

#win32:LIBS += "/NOLOGO"

MOC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
RCC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
UI_DIR = "$$PWD/../../obj/qmake/qt-json-widget"

LIBS += libQtJsonEditorDialog
LIBS += libQtJsonWidget

CONFIG(debug, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Debug"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Debug/qt-json-widget"

    LIBS += "-L$$PWD/../../bin/qmake/x32/Debug"
}

CONFIG(release, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Release"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Release/qt-json-widget"

    LIBS += "-L$$PWD/../../bin/qmake/x32/Release"
}
