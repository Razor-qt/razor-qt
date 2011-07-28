#include <QtGui/QApplication>

#include "sessionconfigwindow.h"


int main(int argc, char** argv)
{
    // TODO/FIXME: maybe singleapp?
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));


    SessionConfigWindow w;
    w.show();

    return a.exec();
}