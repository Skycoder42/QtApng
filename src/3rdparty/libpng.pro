TEMPLATE = lib

TARGET = png

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../lib/ -lz
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../lib/ -lzd
else:unix: LIBS += -L$$OUT_PWD/../../../lib/ -lz

INCLUDEPATH += $$PWD/../zlib
DEPENDPATH += $$PWD/../zlib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../lib/libz.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../lib/libzd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../lib/z.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../lib/zd.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../lib/libz.a
