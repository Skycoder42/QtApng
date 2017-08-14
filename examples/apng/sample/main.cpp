#include "widget.h"
#include <QApplication>
#include <QFile>

//hack to load the plugin
static void ln_plugin() {
	Q_ASSERT(QFile::copy(QStringLiteral(OUTDIR) + QStringLiteral("../../../plugins/imageformats/libqapng.so"),
						QStringLiteral(PLGDIR) + QStringLiteral("imageformats/libqapng.so")));
	qAddPostRoutine([](){
		Q_ASSERT(QFile::remove(QStringLiteral(PLGDIR) + QStringLiteral("imageformats/libqapng.so")));
	});
}
Q_COREAPP_STARTUP_FUNCTION(ln_plugin)

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();
}
