#ifndef RAZORSPINBUTTON_CPP
#define RAZORSPINBUTTON_CPP
#include "razorspinbutton.h"
#include "razor.h"

RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorSpinButton * ret = new RazorSpinButton(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


RazorSpinButton::RazorSpinButton(RazorBar * panel, QWidget * parent, const QString & _cmd)
    : RazorPlugin(panel, parent, _cmd)
{
    cfg = new ReadSettings("spinbutton", this);
    gui = new RazorSpinButtonGUI(this);

    mainLayout()->addWidget(gui);

    // TODO/FIXME: it's the same code as in quicklauncher!
    QSettings *s = cfg->settings();
    s->beginGroup(_cmd);
    int count = s->beginReadArray("apps");

    QString execname;
    QString exec;
    QString icon;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        execname = s->value("name", "").toString();
        exec = s->value("exec", "").toString();
        icon = s->value("icon", "").toString();
        //m_icons[name] = icon;

        if (!QFile::exists(icon))
        {
            qDebug() << "Icon file" << icon << "does not exists. Skipped.";
            continue;
        }
        QAction* tmp = new QAction(QIcon(icon), execname, gui);
        tmp->setData(exec);
        gui->addAction(tmp);
    }
    s->endArray();
    s->endGroup();

    gui->changeAction(0);
}

RazorSpinButton::~RazorSpinButton()
{
//    delete gui;
}

void RazorSpinButtonGUI::mousePressEvent(QMouseEvent* _event)
{
    QToolButton::mousePressEvent(_event);
}

RazorSpinButtonGUI::RazorSpinButtonGUI(RazorSpinButton* parent)
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
    setFixedWidth(100);
    show();
}

void RazorSpinButtonGUI::addAction(QAction* _entrytoadd)
{
    actionList.append(_entrytoadd);
    actionChoose->addAction(_entrytoadd);
}

void RazorSpinButtonGUI::changeAction(int _newstate)
{
    currentAction = actionList.at(_newstate);
    menu()->setActiveAction(actionList.at(index));
    setDefaultAction(currentAction);
    emit actionChanged(_newstate);
}

void RazorSpinButtonGUI::execAction(QAction* _action)
{
    qDebug() << "execAction triggered with" << _action->data();
    QProcess::startDetached(_action->data().toString());
    setDefaultAction(_action);
    index = actionList.indexOf(_action);
    emit actionChanged(index);
}



void RazorSpinButtonGUI::wheelEvent(QWheelEvent* _event)
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

RazorSpinButtonGUI::~RazorSpinButtonGUI()
{
    delete hideTimer;
    for (int i=0; i < actionList.count(); i++)
    {
        delete actionList.at(i);
    }
    delete actionChoose;
}


#endif
