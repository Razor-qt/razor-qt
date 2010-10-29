#ifndef RAZORSPINBUTTON_CPP
#define RAZORSPINBUTTON_CPP
#include "razorspinbutton.h"
#include "razor.h"
bool Razorspinbutton::handleEvent(XEvent* _event)
{
    return Razorplugin::handleEvent(_event);
}

Razorspinbutton::Razorspinbutton(QString _cmd, int _bar): Razorplugin(_bar)
{

    QString cmd = _cmd;
    cmd.remove("razorspinbutton");
    settings = new Readsettings("spin"+cmd+".conf");
    int stateCount = settings->getInt("count");
    gui = new Razorspinbuttongui(this);
    for (int i = 0; i < stateCount; i++)
    {
        QString s;
        s.setNum(i);
        QStringList Explode = settings->getString("state"+s).split("|");
	QAction* tmp = new QAction((QIcon)Explode.at(2),Explode.at(1),gui);
	tmp->setData(Explode.at(0));
	gui->addAction(tmp);
    }
    gui->changeAction(0);
    gui->setFixedHeight(Razor::getInstance().get_looknfeel()->getInt("razorbar_height")-6);
    Razor::getInstance().get_gui()->addWidget(gui,_bar,0,Qt::AlignLeft);
}

Razorspinbutton::~Razorspinbutton()
{
    delete gui;
    delete settings;
}

void Razorspinbuttongui::mousePressEvent(QMouseEvent* _event)
{
    QToolButton::mousePressEvent(_event);
}

Razorspinbuttongui::Razorspinbuttongui(Razorspinbutton* _owner)
{
    actionChoose = new QMenu(this);
    hideTimer = new QTimer(this);
    connect(hideTimer,SIGNAL(timeout()),actionChoose,SLOT(hide()));
    connect(hideTimer,SIGNAL(timeout()),hideTimer,SLOT(stop()));
    connect(this,SIGNAL(triggered(QAction*)),this, SLOT(execAction(QAction*)));
    setMenu(actionChoose);
    setPopupMode(QToolButton::MenuButtonPopup);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    index = 0;
    owner = _owner;
    setFixedWidth(100);
    show();
}

void Razorspinbuttongui::addAction(QAction* _entrytoadd)
{
   actionList.append(_entrytoadd);
   actionChoose->addAction(_entrytoadd);
}

void Razorspinbuttongui::changeAction(int _newstate)
{
    currentAction = actionList.at(_newstate);
    menu()->setActiveAction(actionList.at(index));
    setDefaultAction(currentAction);
    emit actionChanged(_newstate);
}

void Razorspinbuttongui::execAction(QAction* _action)
{
  qDebug() << "execAction triggered with" << _action->data();
  QProcess::startDetached(_action->data().toString());
  setDefaultAction(_action);
  index = actionList.indexOf(_action);
  emit actionChanged(index);
}



void Razorspinbuttongui::wheelEvent(QWheelEvent* _event)
{
    if (menu()->isHidden())
      menu()->show();
    if (hideTimer->isActive())
      hideTimer->stop();
    hideTimer->start(700);
    int numDegrees = _event->delta() / 8;
    int numSteps = numDegrees / 15;
    if (numSteps < 0 && actionList.count()>0)
    {
        qDebug() << "Wheelevent: index going up" << index;
        if ((index+1) < actionList.count())
        {
            index++;
            changeAction(index);
        }
        else
        {
            index = 0;
            changeAction(index);
        }
    }
    else if (numSteps > 0 && actionList.count()>0)
    {
        qDebug() << "Wheelevent: index going down" << index;
        if (index > 0)
        {
            index--;
            changeAction(index);
        }
        else if (index == 0)
        {
            index = actionList.count();
	    index--;
            changeAction(index);
        }

    }
}

Razorspinbuttongui::~Razorspinbuttongui()
{
  delete hideTimer;
  for (int i=0; i < actionList.count(); i++)
  {
    delete actionList.at(i);
  }
  delete actionChoose;
}


#endif
