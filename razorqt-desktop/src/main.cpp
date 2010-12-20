#include "defs.h"
#include "razor.h"
#include "razordeskicon.h"
#include "razordeskmanl.h"
#include "razorworkspace.h"
#include <razorqt/readsettings.h>


void legacyWallPaper()
{
    //now we got the desktop we need to determine if the user wants a defined picture there
    QString finalPixmap = Razor::getInstance().razorsettings()->settings()->value("background_image", "").toString();
    if (finalPixmap != "internal" && finalPixmap != "")
    {
        qDebug() << "Pixmap-custom ok: " << finalPixmap;
    }
    else //now we want to use the system default - we still need to find that one out though
    {
        finalPixmap = Razor::getInstance().themesettings()->desktopBackground();
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
    app.setStyleSheet(Razor::getInstance().themesettings()->qss());

    if (! Razor::getInstance().razorsettings()->settings()->value("desktop_workspace", false).toBool())
    {
        qDebug() << "Using root-window as wallpaper-canvas";
        legacyWallPaper();
        RazorDeskManagerLegacy* deskman=new RazorDeskManagerLegacy(NULL);
        Q_UNUSED(deskman);
    }
    else
    {
        qDebug() << "Making workspace";
        RazorWorkSpaceManager* workman = new RazorWorkSpaceManager(); //theres work to be done here!
        qDebug() << "making icons";
        RazorDeskManagerLegacy* deskman = new RazorDeskManagerLegacy(workman->getWorkSpace());
        Q_UNUSED(deskman);
    }

    return app.exec();
}

