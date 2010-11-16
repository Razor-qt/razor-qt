#ifndef RAZORCMD_H
#define RAZORCMD_H
#include "defs.h"
#include "razorplugin.h"

/**
 * @file razorcmd.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razorcmd and Razorcmdgui
 */

class RazorCmd;

/**
 * @brief The gui-element of Razorcmd
 */
class RazorCmdGUI : public QLineEdit
{
    Q_OBJECT
public:
    RazorCmdGUI(RazorCmd* _owner);
    RazorCmdGUI(QWidget* parent = 0);
    ~RazorCmdGUI();
protected:
    void keyPressEvent (QKeyEvent* _event);
    void wheelEvent(QWheelEvent* _event);
private:
    int index;
    RazorCmd* owner;
};



/**
 * @brief a little textedit-based applauncher for systems without XDG (like *BSD or *NIX or old linux)
 */
class RazorCmd : public RazorPlugin
{
public:
    RazorCmd(int _bar);
    ~RazorCmd();
    virtual bool handleEvent(XEvent* _event);
    void executeCmd(QString _cmd);
    int getOldCmdCount()
    {
        return oldCmd.count();
    }
    QString getOldCmd(int _index)
    {
        return oldCmd.at(_index);
    }
private:
    RazorCmdGUI* gui;
    QList<QString> oldCmd;
};


#endif
