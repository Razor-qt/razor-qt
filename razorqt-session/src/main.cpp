#include <QApplication>
#include <QtGui>
#include <razorqt/readsettings.h>
#include "razorsplash.h"
#include "razorstate.h"
#include "razormodman.h"

/**
* @file main.cpp
* @author Christopher "VdoP" Regali
* @brief just starts the sub-apps and in future maybe saves the windowstates
*/

/**
* @brief our main function doing the loading
*/

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    ReadSettings cfg("razor");
    ReadTheme theme(cfg.settings()->value("theme", "").toString());

    QString splashPixmap(theme.splashScreen());
    if (!splashPixmap.isEmpty())
    {
        // TODO/FIXME: personally I don't like it's waiting here. Configurable?
        // we are fast so there is no reason for splash at all
        // -- petr
        RazorSplash * splash = new RazorSplash(splashPixmap, 5);
    }

    RazorModuleManager modman;
    return app.exec();
}
