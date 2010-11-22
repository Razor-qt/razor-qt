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
    RazorSpinButtonGUI(RazorSpinButton* parent);
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
    int index;
};

class RazorSpinButton : public RazorPlugin
{
    Q_OBJECT
public:
    RazorSpinButton(QString _cmd, RazorBar * panel, QWidget * parent);
    ~RazorSpinButton();

    int widthForHeight(int h)
    {
        return width();
    }
    int heightForWidth(int w)
    {
        return w;
    }
    RazorPlugin::RazorPluginSizing sizePriority()
    {
        return RazorPlugin::Static;
    }

private:
    RazorSpinButtonGUI* gui;
    ReadSettings* settings;

};


#endif
