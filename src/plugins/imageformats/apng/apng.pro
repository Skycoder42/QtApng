TARGET  = qapng

HEADERS += apngimageplugin.h \
    apngreader_p.h \
    apngimagehandler_p.h

SOURCES += apngimageplugin.cpp \
	apngimagehandler.cpp \
	apngreader.cpp

OTHER_FILES += qapng.json \
	../../../3rdparty/build_libpng_static.sh

!libpng_static {
	CONFIG += link_pkgconfig
	unix:PKGCONFIG += libpng
	else:LIBS += -lpng
} else {
	# link against libpng and zlib
	win32:CONFIG(release, debug|release): LIBS_PRIVATE += -L$$OUT_PWD/../../../../lib/ -lpng -lz
	else:win32:CONFIG(debug, debug|release): LIBS_PRIVATE += -L$$OUT_PWD/../../../../lib/ -lpngd -lzd
	else:unix: LIBS_PRIVATE += -L$$OUT_PWD/../../../../lib/ -lpng -lz

	INCLUDEPATH += $$PWD/../../../3rdparty/libpng
	DEPENDPATH += $$PWD/../../../3rdparty/libpng

	win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/libpng.a
	else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/libpngd.a
	else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/png.lib
	else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/pngd.lib
	else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../../lib/libpng.a
}

PLUGIN_TYPE = imageformats
PLUGIN_CLASS_NAME = ApngImagePlugin
load(qt_plugin)
