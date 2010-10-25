#ifndef RAZORCMD_CPP
#define RAZORCMD_CPP
#include "razorcmd.h"
#include "razor.h"

bool Razorcmd::handleEvent(XEvent* _event)
{
    return Razorplugin::handleEvent(_event);
}

Razorcmd::Razorcmd(int _bar): Razorplugin(_bar)
{
    gui = new Razorcmdgui(this);
    //now add us to the bar
    Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignLeft);
}

Razorcmd::~Razorcmd()
{
  delete gui;
}

/**
 * @todo Read the comments in razorcmd.cpp ! there seems to be a real problem here
 */
Razorcmdgui::Razorcmdgui(Razorcmd* _owner)
{
 /* This Plugin is actually NOT working! The question is why! it seems to have something todo with the _NET_WM_WINDOW_TYPE_DOCK flag beeing set for the dock
  * It makes openbox and other wms treat this panel right, but also disables any keyboard input so that this widget cannot work as planned - maybe there is a workaround as this plugin is already finished.
  * If not... my bad then :)
  */
  
  setAttribute(Qt::WA_X11NetWmWindowTypeDialog);
  owner = _owner;
  index=0;
  show();
}

Razorcmdgui::~Razorcmdgui()
{
  
}

void Razorcmd::executeCmd(QString _cmd)
{
  QProcess::startDetached(_cmd);
  oldCmd.append(_cmd);
}

Razorcmdgui::Razorcmdgui(QWidget* parent): QLineEdit(parent)
{

}


void Razorcmdgui::keyPressEvent(QKeyEvent* _event)
{
  qDebug() << "keyPressed!";
  if (_event->key() == Qt::Key_Return)
  {
    QString cmd = text();
    owner->executeCmd(cmd);
    setText("");
    index=0;
  }
  else if (_event->key() == Qt::Key_PageUp)
  {
    if (index <= owner->getOldCmdCount())
    {
      setText(owner->getOldCmd(index));
      index++;
    }
  }
  else if (_event->key() == Qt::Key_PageDown)
  {
    if (index > 0)
    {
      setText(owner->getOldCmd(index));
      index--;
    }
  }
    QLineEdit::keyPressEvent(_event);
}

void Razorcmdgui::wheelEvent(QWheelEvent* _event)
{
    int numDegrees = _event->delta() / 8;
    int numSteps = numDegrees / 15;
    if (numSteps < 0 && owner->getOldCmdCount()>0)
    {
      if (index <= owner->getOldCmdCount())
      {
	setText(owner->getOldCmd(index));
	index++;	
      }
    }
    else if (numSteps > 0 && owner->getOldCmdCount()>0)
    {
      if (index > 0)
      {
	setText(owner->getOldCmd(index));
	index--;
      }
    }
}




#endif

