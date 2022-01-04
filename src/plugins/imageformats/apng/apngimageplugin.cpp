#include "apngimageplugin.h"
#include "apngimagehandler_p.h"
#include "apngreader_p.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	// when format is apng, return CanRead directly.
	if (format == "apng")
		return CanRead;
	if (!format.isEmpty())
		return static_cast<Capability>(0);
	if (!device->isOpen())
		return static_cast<Capability>(0);

	// formats can be empty, thus we need to detect the content inside device.
	if (device->isReadable() && device->bytesAvailable() >= 8) {
		return ApngReader::checkPngSig(device) ? CanRead : static_cast<Capability>(0);
	}

	return static_cast<Capability>(0);
}

QImageIOHandler *ApngImagePlugin::create(QIODevice *device, const QByteArray &format) const
{
	if(device && capabilities(device, format).testFlag(CanRead)) {
		auto handler = new ApngImageHandler();
		handler->setDevice(device);
		handler->setFormat(format);
		return handler;
	} else
		return nullptr;
}
