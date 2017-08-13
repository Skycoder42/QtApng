TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += plugins

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
