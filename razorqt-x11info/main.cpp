#include <QtGui/QApplication>
#include <QtGui/QX11Info>
#include <QtGui/QDesktopWidget>
#include <QtDebug>


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    qDebug() << "Razor-qt environment summary\n";

#ifdef Q_WS_X11
    // collect QX11
    qDebug() << "* X11 Information";
    QX11Info x11;
    qDebug() << "X11 display:\t\t" << x11.display();
    qDebug() << "X11 colormap:\t\t" << x11.colormap();
    qDebug() << "X11 depth:\t\t" << x11.depth();
    qDebug() << "X11 composite:\t\t" << x11.isCompositingManagerRunning();
#endif

    qDebug() << "* QDesktopWidget Information";
    QDesktopWidget *dw = QApplication::desktop();
    qDebug() << "QD virtual desktop:\t" << dw->isVirtualDesktop();
    qDebug() << "QD primary screen:\t" << dw->primaryScreen();
    qDebug() << "QD screen count:\t" << dw->screenCount();
    qDebug() << "** QD screens";
    for (int i = 0; i < dw->screenCount(); ++i)
    {
        qDebug() << "\tScreen" << i << "availableGeometry:\t" << dw->availableGeometry(i);
        qDebug() << "\tScreen" << i << "screenGeometry:\t" << dw->screenGeometry(i);
        qDebug() << "";
    }

    return 0;
}

