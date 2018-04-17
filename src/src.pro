TEMPLATE = subdirs

SUBDIRS += plugins
libpng_static {
	SUBDIRS += 3rdparty
	plugins.depends += 3rdparty
}
