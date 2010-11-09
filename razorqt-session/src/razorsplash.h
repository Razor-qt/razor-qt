#ifndef RAZORSPLASH_H
#define RAZORSPLASH_H

#include "defs.h"
#include <razorqt/readsettings.h>
#include "razorlabel.h"

/**
*@file razorsplash.h
*@brief declares class Razorsplash
*@author Christopher "VdoP" Regali
*/


/**
*@brief manages the splash-screen
*/
class Razorsplash:public QObject
{
    Q_OBJECT
public:
    Razorsplash(QString _splashtheme);
    ~Razorsplash();

private:
    QTimer* timeout;
    QDialog* splashscreen;
    Razorlabel* splashgfx;
    Readsettings* settings;

};



#endif
