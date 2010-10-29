#ifndef RAZORSPLASH_CPP
#define RAZORSPLASH_CPP
#include "razorsplash.h"
/**
*@file razorsplash.cpp
*@brief implements class Razorsplash
*@author Christopher "VdoP" Regali
*/



/**
*@brief constructs everything and shows the splash
*@todo create a class derived from QLabel that emits a clicked() event!
*/
Razorsplash::Razorsplash(QString _splashtheme)
{
  //read settings from given theme
  settings = new Readsettings(_splashtheme);

  QString splashpic = settings->getPath() +  settings->getString("splashpic");
  int splashtimeout = settings->getInt("splashtime");
  int hideonclick = settings->getInt("hideonclick");

  splashscreen = new QDialog;
  splashscreen->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
  splashscreen->setAttribute(Qt::WA_X11NetWmWindowTypeSplash);

  //now set a size of the splashscreen
  splashscreen->setFixedHeight(QApplication::desktop()->height()*.25);
  splashscreen->setFixedWidth(2*splashscreen->height());
  splashscreen->move((QApplication::desktop()->width()-splashscreen->width())/2, (QApplication::desktop()->height()-splashscreen->height())/2);
  splashscreen->show();

  splashgfx = new Razorlabel(splashscreen);
  splashgfx->move(0,0);
  splashgfx->setFixedSize(splashscreen->width(),splashscreen->height());

  splashgfx->setPixmap(((QPixmap)splashpic).scaled(splashscreen->width(),splashscreen->height()));
  splashgfx->show();

  //setup the timing
  timeout = new QTimer;
  timeout->setSingleShot(true);
  connect (timeout, SIGNAL(timeout()),splashscreen,SLOT(hide()));
  timeout->start(splashtimeout);
  //hide on click!
  if (hideonclick != 0)
    connect(splashgfx,SIGNAL(clicked()),splashscreen,SLOT(hide()));

}

/**
*@brief cleans up the mess
*/
Razorsplash::~Razorsplash()
{
  splashscreen->hide();
  delete splashgfx;
  delete timeout;
  delete splashscreen;
  delete settings;
}


#endif
