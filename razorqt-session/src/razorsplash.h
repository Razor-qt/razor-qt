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
class RazorSplash:public QObject
{
    Q_OBJECT
public:
    RazorSplash(QString _splashtheme);
    ~RazorSplash();

private:
    QTimer* timeout;
    QDialog* splashscreen;
    RazorLabel* splashgfx;
    ReadSettings* settings;

};



#endif
