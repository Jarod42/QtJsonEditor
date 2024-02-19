TEMPLATE = lib

CONFIG += shared c++17 uiplugin
QT += widgets # core gui defaulted

INCLUDEPATH += ".."

RESOURCES += QtJsonWidgetPlugin.qrc
HEADERS += QtJsonWidgetPlugin.h
SOURCES += QtJsonWidgetPlugin.cpp

#win32:LIBS += "/NOLOGO"
win32:DEFINES += WIN32
debug:DEFINES += DEBUG
release:DEFINES += NDEBUG

LIBS += libQtJsonWidget

target.path = "$$[QT_INSTALL_PLUGINS]/designer"
INSTALLS += target

MOC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
RCC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
UI_DIR = "$$PWD/../../obj/qmake/qt-json-widget"

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
