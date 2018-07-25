#include "widget.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
#ifdef MODULE_TREE_BUILD
	qDebug() << "Make shure you run \"make install\" before this example";
#endif

	QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();
}
