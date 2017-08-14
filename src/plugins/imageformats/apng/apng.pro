TARGET  = qapng

HEADERS += apngimageplugin.h \
	apngimagehandler.h \
	apngreader.h

SOURCES += apngimageplugin.cpp \
	apngimagehandler.cpp \
	apngreader.cpp

OTHER_FILES += qapng.json \
	build_libpng_static.sh

CONFIG += link_pkgconfig
!libpng_static {
	unix:PKGCONFIG += libpng
	else:LIBS += -lpng
} else {
	win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../3rdparty/libpng/lib/ -lpng
	else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../3rdparty/libpng/lib/ -lpngd
	else:unix: LIBS += -L$$PWD/../../../3rdparty/libpng/lib/ -lpng

	INCLUDEPATH += $$PWD/../../../3rdparty/libpng/include
	DEPENDPATH += $$PWD/../../../3rdparty/libpng/include

	win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../3rdparty/libpng/lib/libpng.a
	else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../3rdparty/libpng/lib/libpngd.a
	else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../3rdparty/libpng/lib/png.lib
	else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../3rdparty/libpng/lib/pngd.lib
	else:unix: PRE_TARGETDEPS += $$PWD/../../../3rdparty/libpng/lib/libpng.a
}

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = ApngImagePlugin
load(qt_plugin)
