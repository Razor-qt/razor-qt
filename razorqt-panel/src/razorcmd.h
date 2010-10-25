#ifndef RAZORCMD_H
#define RAZORCMD_H
#include "defs.h"
#include "razorplugin.h"

/**
 * @file razorcmd.h
 * @author Christopher "VdoP" Regali
 * @brief declares Razorcmd and Razorcmdgui
 */

class Razorcmd;

/**
 * @brief The gui-element of Razorcmd
 */
class Razorcmdgui : public QLineEdit
{
  Q_OBJECT
public:
  Razorcmdgui(Razorcmd* _owner);
  Razorcmdgui(QWidget* parent = 0);
  ~Razorcmdgui();
protected:
  void keyPressEvent (QKeyEvent* _event);
  void wheelEvent(QWheelEvent* _event);
private:
  int index;
  Razorcmd* owner;
};



/**
 * @brief a little textedit-based applauncher for systems without XDG (like *BSD or *NIX or old linux)
 */
class Razorcmd : public Razorplugin
{
public:
  Razorcmd(int _bar);
  ~Razorcmd();
  virtual bool handleEvent(XEvent* _event);
  void executeCmd(QString _cmd);
  int getOldCmdCount() {return oldCmd.count();}
  QString getOldCmd(int _index) {return oldCmd.at(_index);}
private:
  Razorcmdgui* gui;
  QList<QString> oldCmd;
};


#endif
