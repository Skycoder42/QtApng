TEMPLATE = subdirs

SUBDIRS = apng

docTarget.target = doxygen
QMAKE_EXTRA_TARGETS += docTarget
