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


class RazorSpinButton;

/*struct Razorspinbuttonentry
{
  QString name;
  QString cmd;
  QString icon;
};*/

class RazorSpinButtonGUI : public QToolButton
{
    Q_OBJECT
public:
    RazorSpinButtonGUI(RazorSpinButton* _owner);
    ~RazorSpinButtonGUI();
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
    RazorSpinButton* owner;
    int index;
};

class RazorSpinButton :public QObject, public RazorPlugin
{
    Q_OBJECT
public:
    RazorSpinButton(QString _cmd, int _bar);
    ~RazorSpinButton();
    virtual bool handleEvent(XEvent* _event);
private:
    RazorSpinButtonGUI* gui;
    ReadSettings* settings;

};


#endif
