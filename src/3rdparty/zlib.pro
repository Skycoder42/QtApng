TEMPLATE = lib

TARGET = z

QT =

CONFIG -= qt
CONFIG += \
	static \
	hide_symbols \
	exceptions_off rtti_off warn_off

MODULE_INCLUDEPATH = $$PWD
load(qt_helper_lib)

HEADERS += $$files(*.h)
SOURCES += $$files(*.c)
