TEMPLATE = lib

TARGET = qtapng_png

QT =

CONFIG -= qt
CONFIG += static hide_symbols exceptions_off rtti_off warn_off

android:equals(ANDROID_TARGET_ARCH, armeabi-v7a) {
	QMAKE_CFLAGS -= -mfpu=vfp
	QMAKE_CFLAGS += -mfpu=neon
	QMAKE_CXXFLAGS -= -mfpu=vfp
	QMAKE_CXXFLAGS += -mfpu=neon
	CONFIG += neon
}

HEADERS += $$files(src/*.h)
SOURCES += $$files(src/*.c)
SOURCES -= src/example.c

MODULE_INCLUDEPATH = $$PWD/src
load(qt_helper_lib)

QMAKE_USE_PRIVATE += apng_zlib
