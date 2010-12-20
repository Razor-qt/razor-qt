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
        QSplashScreen splash(splashPixmap);
    }

    RazorModuleManager modman;
    // TODO/FIXME: there was waiting for 5 sec to show splash longer...
    // personally I don't like it, but it;s no prob to return it again
    return app.exec();
}
