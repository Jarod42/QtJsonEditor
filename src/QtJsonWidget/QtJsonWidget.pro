TEMPLATE = lib

CONFIG += static c++17
QT += widgets # core gui defaulted

INCLUDEPATH += ".."
INCLUDEPATH += "../../submodules/valijson/include"

HEADERS += \
	IJsonWidget.h \
	JsonReferenceResolver.h \
	QJsonWidget.h \
	array/ArrayGridLayoutWidget.h \
	boolean/CheckBoxWidget.h \
	integer/IntegerLineEditWidget.h \
	integer/SpinBoxWidget.h \
	jsonKeys.h \
	null/LabelWidget.h \
	number/NumberLineEditWidget.h \
	object/ObjectGridLayoutWidget.h \
	string/StringLineEditWidget.h \
	string/color/ColorDialogButtonWidget.h \
	string/date/CalendarWidget.h \
	string/date/DateEditWidget.h \
	string/enum/ComboBoxWidget.h

SOURCES += \
	IJsonWidget.cpp \
	JsonReferenceResolver.cpp \
	QJsonWidget.cpp \
	array/ArrayGridLayoutWidget.cpp \
	boolean/CheckBoxWidget.cpp \
	integer/IntegerLineEditWidget.cpp \
	integer/SpinBoxWidget.cpp \
	null/LabelWidget.cpp \
	number/NumberLineEditWidget.cpp \
	object/ObjectGridLayoutWidget.cpp \
	string/StringLineEditWidget.cpp \
	string/color/ColorDialogButtonWidget.cpp \
	string/date/CalendarWidget.cpp \
	string/date/DateEditWidget.cpp \
	string/enum/ComboBoxWidget.cpp

debug:DEFINES += DEBUG
release:DEFINES += NDEBUG

MOC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
RCC_DIR = "$$PWD/../../obj/qmake/qt-json-widget"
UI_DIR = "$$PWD/../../obj/qmake/qt-json-widget"

CONFIG(debug, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Debug"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Debug/qt-json-widget"
}

CONFIG(release, debug|release) {
    DESTDIR = "$$PWD/../../bin/qmake/x32/Release"
    OBJECTS_DIR = "$$PWD/../../obj/qmake/x32/Release/qt-json-widget"
}
