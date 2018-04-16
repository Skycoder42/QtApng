TEMPLATE = subdirs

SUBDIRS += zlib libpng

libpng.depends += zlib
