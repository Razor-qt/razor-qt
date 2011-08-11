#include <QtGui/QApplication>

#include "desktopconfigwindow.h"


int main(int argc, char** argv)
{
    // TODO/FIXME: maybe singleapp?
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));


    DesktopConfigWindow w;
    w.show();

    return a.exec();
}