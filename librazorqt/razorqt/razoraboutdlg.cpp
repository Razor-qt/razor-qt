#ifndef RAZORABOUTDLG_CPP
#define RAZORABOUTDLG_CPP
#include "razoraboutdlg.h"
#include "ui_aboutdlg.h"
#include <razorqt/readsettings.h>
#include <QDebug>
/**
 * @file razoraboutdlg.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements the about dialog
 */

/**
 * @brief prepares the data to show and fills the form, then shows.
 */
class RazorAboutDLGPrivate: public Ui_about, public QDialog
{
public:
    RazorAboutDLGPrivate();
};


//
RazorAboutDLGPrivate::RazorAboutDLGPrivate()
{
    //ReadSettings mainSettings("razor.conf");
    //ReadSettings looknfeel(mainSettings.getString("style_theme"));

    setupUi(this);
    QString tmp = "Version: ";
    tmp.append("SVN");
    versionLabel->setText(tmp);
    aboutText->setReadOnly(true);
    aboutText->insertPlainText("The razor-qt Crew consists of (in alphabetical order):  \n \n");
    
    aboutText->insertPlainText("Christopher 'VdoP' Regali  \n Alexander Sokoloff \n Petr Vanek \n \n ");
    
    aboutText->insertPlainText("If you want to contribute, just visit razor-qt.sf.net \n \n");
    
    
    
    aboutText->insertPlainText("Special thanks to: \n Eugene Pivnev (qtDesktop) \n Paul Gnther (for desktop backgrounds) \n Manuel Meier (for ideas) \n Alexander Zakher (for the name) \n and KDE (http://www.kde.org) \n \n \n");
    aboutText->insertPlainText("License: GPL \n");

    this->setAttribute(Qt::WA_DeleteOnClose);
    show();
    razorLabel->setScaledContents(true);
    // TODO/FIXME: icon from theme
    //qDebug() << ((looknfeel.getPath()+looknfeel.getString("razormainmenu_icon")));
    //razorLabel->setPixmap(((QPixmap)(looknfeel.getPath()+looknfeel.getString("razormainmenu_icon"))));
}


RazorAboutDLG::RazorAboutDLG()
{
    d_ptr = new RazorAboutDLGPrivate();
}




#endif
