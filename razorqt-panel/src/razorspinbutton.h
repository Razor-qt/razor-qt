#ifndef RAZORSPINBUTTON_H
#define RAZORSPINBUTTON_H
#include "defs.h"
#include "razorplugin.h"
#include <razorqt/readsettings.h>

/**
 * @file razorspinbutton.h
 * @author Christopher "VdoP" Regali
 * @brief this declares Razorspinbutton and Razorspinbuttongui
 */


class Razorspinbutton;

/*struct Razorspinbuttonentry
{
  QString name;
  QString cmd;
  QString icon;
};*/

class Razorspinbuttongui : public QToolButton
{
    Q_OBJECT
public:
    Razorspinbuttongui(Razorspinbutton* _owner);
    ~Razorspinbuttongui();
    void addAction(QAction*);
protected:
    virtual void wheelEvent(QWheelEvent* );
    virtual void mousePressEvent(QMouseEvent* );
signals:
    void actionClicked(int);
    void actionChanged(int);
public slots:
    void execAction(QAction*);
    void changeAction(int);

private:
    QTimer* hideTimer;
    QAction* currentAction;
    QMenu* actionChoose;
    QList<QAction*> actionList;
    Razorspinbutton* owner;
    int index;
};

class Razorspinbutton :public QObject, public Razorplugin
{
    Q_OBJECT
public:
    Razorspinbutton(QString _cmd, int _bar);
    ~Razorspinbutton();
    virtual bool handleEvent(XEvent* _event);
private:
    Razorspinbuttongui* gui;
    Readsettings* settings;

};


#endif
