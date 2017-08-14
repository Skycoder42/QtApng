#-------------------------------------------------
#
# Project created by QtCreator 2017-08-14T12:45:16
#
#-------------------------------------------------

QT       += testlib

TARGET = tst_apngplugin
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += \
		tst_apngplugin.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"
DEFINES += OUTDIR=\\\"$$OUT_PWD/\\\"
DEFINES += PLGDIR=\\\"$$[QT_INSTALL_PLUGINS]/\\\"

RESOURCES += \
    tst_apngplugin.qrc
