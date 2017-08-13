#include "apngimageplugin.h"
#include "apngimagehandler.h"
#include "apngreader.h"

ApngImagePlugin::ApngImagePlugin(QObject *parent) :
	QImageIOPlugin(parent)
{}

QImageIOPlugin::Capabilities ApngImagePlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
	if (format == "apng") {
		if(device)
			return ApngReader::checkPngSig(device) ? CanRead : (Capability)0;
		else
			return CanRead;
	} else
		return 0;
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
