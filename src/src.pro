TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += plugins
libpng_static {
	SUBDIRS += 3rdparty
	plugins.depends += 3rdparty
}
