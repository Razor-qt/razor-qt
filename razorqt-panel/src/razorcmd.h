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
    RazorCmdGUI(RazorCmd* parent);
    ~RazorCmdGUI();
protected:
    void keyPressEvent (QKeyEvent* _event);
    void wheelEvent(QWheelEvent* _event);
private:
    int index;
    QList<QString> oldCmd;

    int getOldCmdCount()
    {
        return oldCmd.count();
    }

    QString getOldCmd(int _index)
    {
        return oldCmd.at(_index);
    }
    void executeCmd(QString _cmd);
};



/**
 * @brief a little textedit-based applauncher for systems without XDG (like *BSD or *NIX or old linux)
 */
class RazorCmd : public RazorPlugin
{
    Q_OBJECT
public:
    RazorCmd(RazorBar * panel, QWidget * parent);
    ~RazorCmd();

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
    RazorCmdGUI* gui;
};


#endif
