#ifndef RAZORWORKSPACE_CPP
#define RAZORWORKSPACE_CPP
/**
 * @file razorworkspace.cpp
 * @brief implements Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */
#include "razorworkspace.h"
#include "razor.h"
#include <razorqt/readsettings.h>

void Razorworkspace::mouseMoveEvent(QMouseEvent* _ev)
{
    QLabel::mouseMoveEvent(_ev);
}

void Razorworkspace::mousePressEvent(QMouseEvent* _ev)
{
    QLabel::mousePressEvent(_ev);
}

void Razorworkspace::mouseReleaseEvent(QMouseEvent* _ev)
{
    emit mouseReleased(_ev);
    QLabel::mouseReleaseEvent(_ev);
}

void Razorworkspacemanager::switchDesktop(int _change)
{
  Xfitman* xfitman = Razor::getInstance().getxfitman();
  int num_desks = xfitman->getNumDesktop();
  if (_change > 0)
  {
    if ( xfitman->getActiveDesktop()+1 < num_desks)
      xfitman->setActiveDesktop(xfitman->getActiveDesktop()+1);
    else
      xfitman->setActiveDesktop(0);
  }
  if (_change < 0)
  {
    if (xfitman->getActiveDesktop()-1 >= 0)
      xfitman->setActiveDesktop(xfitman->getActiveDesktop()-1);
    else
      xfitman->setActiveDesktop(num_desks-1);
  }
}


void Razorworkspace::wheelEvent(QWheelEvent* _ev)
{
    int numDegrees = _ev->delta() / 8;
    int numSteps = numDegrees / 15;
    emit mouseWheeled(-numSteps);
}





Razorworkspace::Razorworkspace(QWidget* _parent, Qt::WindowFlags _f): QLabel(_parent, _f)
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
  setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
  setScaledContents(true);
  move(0,0);
  show();
}

Razorworkspace::~Razorworkspace()
{

}

void Razorworkspacemanager::mouseClicked(QMouseEvent* _event)
{
  qDebug() << "click-released the workspace";
  //maybe popup a menu or stuff - i dont know
}

Razorworkspacemanager::Razorworkspacemanager(QObject* parent): QObject(parent)
{

  //this may actually make the icon work on multihead
  QWidget* screen = QApplication::desktop()->screen(QApplication::desktop()->primaryScreen());
  workSpace = new Razorworkspace;

  connect(workSpace, SIGNAL(mouseReleased(QMouseEvent*)),this, SLOT(mouseClicked(QMouseEvent*)));
  connect(workSpace, SIGNAL(mouseWheeled(int)), this, SLOT(switchDesktop(int)));


  workSpace->resize(screen->width(),screen->height());



  //now we got the desktop we need to determine if the user wants a defined picture there
  Readsettings customPixmapSettings("desktop.conf");
  QString finalPixmap = customPixmapSettings.getString("deskPixMap");
  if (finalPixmap != "internal" && finalPixmap != "")
  {qDebug() << "Pixmap-custom ok: " << finalPixmap;}
  else //now we want to use the system default - we still need to find that one out though
  {
    qDebug() << "trying to get system-defaults";
    Readsettings* styleSettings = Razor::getInstance().themesettings();
    finalPixmap=styleSettings->getPath() + styleSettings->getString("desktop_background");

    qDebug() << "trying to get system-defaults" << finalPixmap;
  }

  //now try to set it - if its not set we assume the theme stylesheet takes care of it some way (like a gradient or stuff)
  if (finalPixmap != "")
    workSpace->setPixmap(finalPixmap);


}

Razorworkspacemanager::~Razorworkspacemanager()
{
  delete workSpace;
}




#endif
