#include "apngreader.h"
#include <QDebug>
#include <QImage>
#include <QRect>
#include <QThread>
#include <QtEndian>

QHash<png_structp, ApngReader*> ApngReader::_readers;

ApngReader::ApngReader(QObject *parent) :
	QObject(parent),
	_device(nullptr),
	_png(nullptr),
	_info(nullptr),
	_infoRead(false),
	_animated(true),
	_skipFirst(false),
	_imageSize(),
	_frameCount(1),
	_plays(0),
	_frame(),
	_lastImg(),
	_allFrames()
{}

ApngReader::~ApngReader()
{
	if(_png) {
		_readers.remove(_png);
		png_destroy_read_struct(&_png, &_info, NULL);
	}
}

bool ApngReader::checkPngSig(QIODevice *device)
{
	if(device) {
		auto sig = device->peek(8);
		if(png_sig_cmp((png_const_bytep)sig.constData(), 0, sig.size()) == 0)
			return true;
	}
	return false;
}

bool ApngReader::init(QIODevice *device)
{
	if(_device)
		return _infoRead;

	//verify png
	if(checkPngSig(device))
		_device = device;
	else
		return false;

	try {
		//init png structs
		_png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
		if(!_png)
			throw QStringLiteral("failed to create png struct");
		_info = png_create_info_struct(_png);
		if(!_info)
			throw QStringLiteral("failed to create info struct");

		if (setjmp(png_jmpbuf(_png)))
			throw QStringLiteral("failed to read image header");

		_readers.insert(_png, this);
		png_set_progressive_read_fn(_png, NULL, &ApngReader::info_fn, &ApngReader::row_fn, &ApngReader::end_fn);

		//read image sig + header
		auto valid = readChunk(8);
		do {
			valid = readChunk();
		} while(valid && !_infoRead);

		return _infoRead;
	} catch(QString e) {
		qCritical() << e;
		_infoRead = false;
		return false;
	}
}

ApngReader::ApngFrame ApngReader::readFrame(quint32 index)
{
	if(index < (quint32)_allFrames.size())
		return _allFrames[index];

	if (setjmp(png_jmpbuf(_png)))
		return ApngFrame();

	auto valid = false;
	do {
		valid = readChunk();
	} while(valid && index >= (quint32)_allFrames.size());

	if(index < (quint32)_allFrames.size())
		return _allFrames[index];
	else
		return ApngFrame();
}

bool ApngReader::isAnimated() const
{
	return _animated;
}

QSize ApngReader::size() const
{
	return _imageSize;
}

quint32 ApngReader::frames() const
{
	return _frameCount;
}

quint32 ApngReader::plays() const
{
	return _plays;
}

void ApngReader::info_fn(png_structp png_ptr, png_infop info_ptr)
{
	auto reader = _readers[png_ptr];
	Frame &frame = reader->_frame;

	//init png reading
	png_set_expand(png_ptr);
	png_set_strip_16(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
	png_set_bgr(png_ptr);
	(void)png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	//init read frame struct
	frame.x = 0;
	frame.y = 0;
	frame.width = png_get_image_width(png_ptr, info_ptr);
	frame.height = png_get_image_height(png_ptr, info_ptr);
	frame.channels = png_get_channels(png_ptr, info_ptr);
	frame.delay_num = 0;
	frame.delay_den = 10;
	frame.dop = PNG_DISPOSE_OP_NONE;
	frame.bop = PNG_BLEND_OP_SOURCE;
	frame.rowbytes = png_get_rowbytes(png_ptr, info_ptr);
	frame.p = new unsigned char[frame.height * frame.rowbytes];
	frame.rows = new png_bytep[frame.height * sizeof(png_bytep)];
	for (quint32 j = 0; j < frame.height; j++)
		frame.rows[j] = frame.p + j * frame.rowbytes;

	// init image
	reader->_lastImg = QImage(frame.width, frame.height, QImage::Format_ARGB32);
	reader->_lastImg.fill(Qt::black);

	//read apng information
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_acTL)) {
		reader->_animated = true;
		png_get_acTL(png_ptr, info_ptr, &reader->_frameCount, &reader->_plays);
		reader->_skipFirst = png_get_first_frame_is_hidden(png_ptr, info_ptr);

		//add extended APNG read functions
		png_set_progressive_frame_fn(png_ptr, &ApngReader::frame_info_fn, &ApngReader::frame_end_fn);
		//read info for first frame (skipped otherwise)
		if(!reader->_skipFirst)
			frame_info_fn(png_ptr, 0);
	}

	reader->_infoRead = true;
}

void ApngReader::row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
	Q_UNUSED(pass)
	auto reader = _readers[png_ptr];
	png_progressive_combine_row(png_ptr, reader->_frame.rows[row_num], new_row);
}

void ApngReader::end_fn(png_structp png_ptr, png_infop info_ptr)
{
	Q_UNUSED(info_ptr);
	auto reader = _readers[png_ptr];
	Frame &frame = reader->_frame;

	if(!reader->_animated) {
		reader->copyOver();
		reader->_allFrames.append(reader->_lastImg);
	}

	if (frame.p && frame.rows) {
		delete[] frame.rows;
		delete[] frame.p;
	}
}

void ApngReader::frame_info_fn(png_structp png_ptr, png_uint_32 frame_num)
{
	Q_UNUSED(frame_num);
	auto reader = _readers[png_ptr];
	auto info_ptr = reader->_info;
	Frame &frame = reader->_frame;

	frame.x = png_get_next_frame_x_offset(png_ptr, info_ptr);
	frame.y = png_get_next_frame_y_offset(png_ptr, info_ptr);
	frame.width = png_get_next_frame_width(png_ptr, info_ptr);
	frame.height = png_get_next_frame_height(png_ptr, info_ptr);
	frame.delay_num = png_get_next_frame_delay_num(png_ptr, info_ptr);
	frame.delay_den = png_get_next_frame_delay_den(png_ptr, info_ptr);
	frame.dop = png_get_next_frame_dispose_op(png_ptr, info_ptr);
	frame.bop = png_get_next_frame_blend_op(png_ptr, info_ptr);
}

void ApngReader::frame_end_fn(png_structp png_ptr, png_uint_32 frame_num)
{
	auto reader = _readers[png_ptr];
	Frame &frame = reader->_frame;
	auto &image = reader->_lastImg;

	if(frame_num == 0 && reader->_skipFirst)//TODO not always, only when reading animated
		return;
	if((frame_num == 0 && !reader->_skipFirst) ||
	   (frame_num == 1 && reader->_skipFirst)) {
		frame.bop = PNG_BLEND_OP_SOURCE;
		if (frame.dop == PNG_DISPOSE_OP_PREVIOUS)
			frame.dop = PNG_DISPOSE_OP_BACKGROUND;
	}

	QImage temp;

	if(frame.dop == PNG_DISPOSE_OP_PREVIOUS)
		temp = image;

	if (frame.bop == PNG_BLEND_OP_OVER)
		reader->blendOver();
	else
		reader->copyOver();

	reader->_allFrames.append({image, (double)frame.delay_num / (double)frame.delay_den});

	if (frame.dop == PNG_DISPOSE_OP_PREVIOUS)
		image = temp;
	else if (frame.dop == PNG_DISPOSE_OP_BACKGROUND) {
		for(quint32 y = 0; y < frame.height; y++) {
			for(quint32 x = 0; x < frame.width; x++)
				image.setPixelColor(x + frame.x, y + frame.y, Qt::black);
		}
	}
}

bool ApngReader::readChunk(quint32 len)
{
	QByteArray data;
	if(len == 0) {// read exactly 1 chunk
		//read 4 bytes -> size
		data = _device->read(4);
		len = *((quint32*)data.constData());
		len = qFromBigEndian(len) + 8;//type (4b) + crc (4b)
		data += _device->read(len);
	} else
		data = _device->read(len);
	if(!data.isEmpty())
		png_process_data(_png, _info, (png_bytep)data.data(), data.size());
	return !_device->atEnd();
}

void ApngReader::copyOver()
{
	for(quint32 y = 0; y < _frame.height; y++) {
		for(quint32 x = 0; x < _frame.width; x++) {
			auto px = x*4;

			QColor c;
			c.setBlue(_frame.rows[y][px]);
			c.setGreen(_frame.rows[y][px+1]);
			c.setRed(_frame.rows[y][px+2]);
			c.setAlpha(_frame.rows[y][px+3]);

			_lastImg.setPixelColor(x + _frame.x, y + _frame.y, c);
		}
	}
}

void ApngReader::blendOver()
{
	for(quint32 y = 0; y < _frame.height; y++) {
		for(quint32 x = 0; x < _frame.width; x++) {
			auto px = x*4;

			QColor src;
			src.setBlue(_frame.rows[y][px]);
			src.setGreen(_frame.rows[y][px+1]);
			src.setRed(_frame.rows[y][px+2]);
			src.setAlpha(_frame.rows[y][px+3]);

			if(src.alpha() == 0xFF)
				_lastImg.setPixelColor(x + _frame.x, y + _frame.y, src);
			else if(src.alpha() != 0x00) {
				auto dst = _lastImg.pixelColor(x + _frame.x, y + _frame.y);

				//do porter-duff blending
				if(dst.alpha() != 0x00) {
					auto u = src.alpha() * 0xFF;
					auto v = (0xFF - src.alpha()) * dst.alpha();
					auto al = u + v;
					src.setBlue((src.blue() * u + dst.blue() * v) / al);
					src.setGreen((src.green() * u + dst.green() * v) / al);
					src.setRed((src.red() * u + dst.red() * v) / al);
					src.setAlpha(al/255);
				}

				_lastImg.setPixelColor(x + _frame.x, y + _frame.y, src);
			}
		}
	}
}



ApngReader::ApngFrame::ApngFrame(const QImage &image, double delay) :
	QImage(image),
	_delay(delay)
{}

double ApngReader::ApngFrame::delay() const
{
	return _delay;
}

int ApngReader::ApngFrame::delayMsec() const
{
	return qRound(_delay * 1000);
}
