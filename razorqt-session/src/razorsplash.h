#ifndef RAZORSPLASH_H
#define RAZORSPLASH_H

#include <QSplashScreen>


/**
*@brief constructs everything and shows the splash
*/
class RazorSplash : public QSplashScreen
{
    Q_OBJECT
public:
    RazorSplash(const QString & paper, int sec);
    ~RazorSplash();

private:
    QTimer* timeout;
};



#endif
