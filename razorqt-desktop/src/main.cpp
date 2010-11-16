#include "defs.h"
#include "razor.h"
#include "razordeskicon.h"
#include "razordeskmanl.h"
#include "razorworkspace.h"
#include <razorqt/readsettings.h>


void legacyWallPaper()
{
    //now we got the desktop we need to determine if the user wants a defined picture there
    ReadSettings customPixmapSettings("desktop.conf");
    QString finalPixmap = customPixmapSettings.getString("deskPixMap");
    if (finalPixmap != "internal" && finalPixmap != "")
    {
        qDebug() << "Pixmap-custom ok: " << finalPixmap;
    }
    else //now we want to use the system default - we still need to find that one out though
    {
        qDebug() << "trying to get system-defaults";
        ReadSettings* styleSettings = Razor::getInstance().themesettings();
        finalPixmap=styleSettings->getPath() + styleSettings->getString("desktop_background");

        qDebug() << "trying to get system-defaults" << finalPixmap;
    }

    qDebug() << finalPixmap;
    //now try to set it - if its not set we assume the theme stylesheet takes care of it some way (like a gradient or stuff)
    if (finalPixmap != "")
    {
        int width,height;
        QDesktopWidget * dw = QApplication::desktop();
        if (dw->screenCount() == 1)
        {
            width=dw->width();
            height = dw->height();
        }
        else
        {
            width=dw->screenGeometry(-1).width();
            height=dw->screenGeometry(-1).height();

        }
        Razor::getInstance().getxfitman()->setRootBackground(((QPixmap) finalPixmap).scaled(width,height));
    }
}


int main (int argc, char* argv[])
{
    //TODO make singleton QApplication that does all this shit, then make razorsettings and stylesettings available and fix all the other stuff with it

    QApplication app(argc,argv);
    QFile stylesheet(Razor::getInstance().themesettings()->getPath() + Razor::getInstance().themesettings()->getString("panel_stylesheet"));
    QString sheet="";
    if (stylesheet.exists())
    {
        stylesheet.open(QIODevice::ReadOnly | QIODevice::Text);
        sheet=stylesheet.readAll();
    }
    qDebug() << "loading sheet: " << stylesheet.fileName() << " with content: " << sheet;
    app.setStyleSheet(sheet);

    if (Razor::getInstance().razorsettings()->getString("desktop_workspace")!= "1")
    {
        qDebug() << "Using root-window as wallpaper-canvas";
        legacyWallPaper();
        RazorDeskManagerLegacy* deskman=new RazorDeskManagerLegacy(NULL);
    }
    else
    {
        qDebug() << "Making workspace";
        RazorWorkSpaceManager* workman = new RazorWorkSpaceManager; //theres work to be done here!
        qDebug() << "making icons";
        RazorDeskManagerLegacy* deskman = new RazorDeskManagerLegacy(workman->getWorkSpace());
    }
    return app.exec();
}

