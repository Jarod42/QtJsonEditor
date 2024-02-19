TEMPLATE = lib

CONFIG += static c++17 lrelease
QT += widgets # core gui defaulted

INCLUDEPATH += ".."
INCLUDEPATH += "../../submodules/valijson/include"

FORMS += EditorDialog.ui 
HEADERS += EditorDialog.h
SOURCES += EditorDialog.cpp
TRANSLATIONS += ../../ts/qtjsoneditor_en.ts
TRANSLATIONS += ../../ts/qtjsoneditor_fr.ts

debug:DEFINES += DEBUG
release:DEFINES += NDEBUG

#win32:LIBS += "/NOLOGO"

MOC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
RCC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
UI_DIR = "$$PWD/../../obj/qmake/qt-json-widget"

CONFIG(debug, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Debug"
    QM_FILES_INSTALL_PATH = "$$PWD/../../bin/qmake/x32/Debug"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Debug/qt-json-widget"
}

CONFIG(release, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Release"
    QM_FILES_INSTALL_PATH = "$$PWD/../../bin/qmake/x32/Release"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Release/qt-json-widget"
}
