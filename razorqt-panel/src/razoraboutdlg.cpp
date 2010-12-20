#ifndef RAZORABOUTDLG_CPP
#define RAZORABOUTDLG_CPP
#include "razoraboutdlg.h"
#include "razor.h"
#include <razorqt/readsettings.h>

/**
 * @file razoraboutdlg.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements the about dialog
 */

/**
 * @brief prepares the data to show and fills the form, then shows.
 */

RazorAboutDLG::RazorAboutDLG()
{
    //ReadSettings mainSettings("razor.conf");
    //ReadSettings looknfeel(mainSettings.getString("style_theme"));

    setupUi(this);
    QString tmp = "Version: ";
    tmp.append(RAZOR_VERSION);
    versionLabel->setText(tmp);
    aboutText->setReadOnly(true);
    aboutText->insertPlainText("Mail me at: chris.vdop@googlemail.com \n \n");
    aboutText->insertPlainText("Special thanks to: \n Eugene Pivnev (qtDesktop) \n Petr Vanek (for patches) \n Paul Gnther (for desktop backgrounds) \n Manuel Meier (for ideas) \n Alexander Zakher (for the name) \n antico (for making me do this) \n and KDE (http://www.kde.org) \n \n \n");
    aboutText->insertPlainText("License: GPL \n");
    show();
    razorLabel->setScaledContents(true);
    // TODO/FIXME: icon from theme
    //qDebug() << ((looknfeel.getPath()+looknfeel.getString("razormainmenu_icon")));
    //razorLabel->setPixmap(((QPixmap)(looknfeel.getPath()+looknfeel.getString("razormainmenu_icon"))));
}





#endif
