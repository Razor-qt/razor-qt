#ifndef RAZORQUICKLAUNCH_H
#define RAZORQUICKLAUNCH_H

#include "razorplugin.h"
#include <razorqt/readsettings.h>


class RazorQuickLaunch;


/*! \brief Implementation of the "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunchGUI : public QWidget
{
	Q_OBJECT
public:
    RazorQuickLaunchGUI(RazorQuickLaunch * _owner);
    ~RazorQuickLaunchGUI();

public slots:
	//! run command in the action
	void execAction(QAction*);
	//! Arrange actions in the grid
	void setupGUI(int height);
  
private:
	RazorQuickLaunch * owner;
};


/*! \brief Loader for "quick launcher" icons in the panel.
\author Petr Vanek <petr@scribus.info>
*/
class RazorQuickLaunch :public QObject, public Razorplugin
{
	Q_OBJECT

public:
    RazorQuickLaunch(QString cmd, int bar);
    ~RazorQuickLaunch();
    virtual bool handleEvent(XEvent* _event);

private:
	RazorQuickLaunchGUI * gui;
	Readsettings * settings;    
};

#endif
