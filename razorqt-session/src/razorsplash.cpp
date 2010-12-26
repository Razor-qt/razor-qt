#include "razorsplash.h"

#include <QTimer>
#include <QtDebug>

RazorSplash::RazorSplash(const QString & paper, int sec)
    : QSplashScreen(paper)
{
    qDebug() << "Splash: init" << paper << "for" << sec << "seconds";
    show();

    //setup the timing
    timeout = new QTimer(this);
    timeout->setSingleShot(true);
    connect (timeout, SIGNAL(timeout()), this, SLOT(hide()));
    timeout->start(sec * 1000);
}

/**
*@brief cleans up the mess
*/
RazorSplash::~RazorSplash()
{
    hide();
}
