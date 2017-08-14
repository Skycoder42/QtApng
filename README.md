# qapng
apng image plugin for Qt to support animated PNGs

## Features
Enable the usage of apng images with Qt. The plugin adds the apng format as a new format for any Qt application, and thus supports loading of apng images via `QMovie`, `AnimatedImage` and other types.

## Requirements
To build the plugin, libpng with the apng patch applied is required. If your system does not provide the patched library, you can use the [`build_libpng_static.sh`](src/3rdparty/build_libpng_static.sh) script to download and build it statically, and automatically set up the project to use this static libpng version for compilation.

## Download/Installation
1. **Arch-Linux only:** If you are building against your system Qt, you can use my AUR-Repository: [qt5-apng-plugin](https://aur.archlinux.org/packages/qt5-apng-plugin/)
2. Build it yourself! **Note:** This requires perl to be installed. If you don't have/need cmake, you can ignore the related warnings. To automatically build and install to your Qt installation, run:
	- `qmake -r`
	- `make`
	- `make install`

## Usage
Simply use the default Qt classes like `QImageReader`, `QMovie` etc.

**Format Detection:**  
Since the png format is already used by Qt, `*.png` files will **not** use the plugin. To load a png as animated, you can either rename the file to `*.apng`, or set the format explicitly
```
QMovie movie("path/to/image.png", "apng");
```