#ifndef APNGREADER_H
#define APNGREADER_H

#include <QObject>
#include <QIODevice>
#include <png.h>
#include <QImage>

#ifndef PNG_APNG_SUPPORTED
#error libpng with APNG patch is required
#endif
#ifndef PNG_READ_APNG_SUPPORTED
#error libpng with APNG patch and APNG read support is required
#endif

class ApngReader : public QObject
{
	Q_OBJECT

public:
	class ApngFrame : public QImage {
	public:
		ApngFrame(const QImage &image = {}, double delay = 0.1);

		double delay() const;
		int delayMsec() const;

	private:
		double _delay;
	};

	explicit ApngReader(QObject *parent = nullptr);
	~ApngReader();

	static bool checkPngSig(QIODevice *device);

	bool init(QIODevice *device);
	ApngFrame readFrame(quint32 index);

	bool isAnimated() const;
	QSize size() const;
	quint32 frames() const;
	quint32 plays() const;

private:
	static QHash<png_structp, ApngReader*> _readers;

	QIODevice *_device;

	png_structp _png;
	png_infop _info;

	//image info
	bool _infoRead;
	bool _animated;
	bool _skipFirst;
	QSize _imageSize;
	quint32 _frameCount;
	quint32 _plays;

	struct Frame {
		quint32 x;
		quint32 y;
		quint32 width;
		quint32 height;
		quint32 channels;

		quint16 delay_num;
		quint16 delay_den;
		quint8 dop;
		quint8 bop;

		quint32 rowbytes;
		unsigned char * p;
		png_bytepp rows;
	} _frame;

	QImage _lastImg;

	QList<ApngFrame> _allFrames;

	static void info_fn(png_structp png_ptr, png_infop info_ptr);
	static void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
	static void end_fn(png_structp png_ptr, png_infop info_ptr);

	static void frame_info_fn(png_structp png_ptr, png_uint_32 frame_num);
	static void frame_end_fn(png_structp png_ptr, png_uint_32 frame_num);

	bool readChunk(quint32 len = 0);
	void copyOver();
	void blendOver();
};

#endif // APNGREADER_H
