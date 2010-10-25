#include "defs.h"
#include "razor.h"
#include "razordeskicon.h"
#include "razordeskmanl.h"
#include "razorworkspace.h"
#include <razorqt/readsettings.h>


int main (int argc, char* argv[])
{
    //TODO make singleton QApplication that does all this shit, then make razorsettings and stylesettings available and fix all the other stuff with it
    
    QApplication app(argc,argv);
    QFile stylesheet(Razor::getInstance().themesettings()->getPath() + Razor::getInstance().themesettings()->getValue("panel_stylesheet"));
    QString sheet="";
    if (stylesheet.exists())
    {
        stylesheet.open(QIODevice::ReadOnly | QIODevice::Text);
        sheet=stylesheet.readAll();
    }
    qDebug() << "loading sheet: " << stylesheet.fileName() << " with content: " << sheet;
    app.setStyleSheet(sheet);


    qDebug() << "Making workspace";
    Razorworkspacemanager workman; //theres work to be done here!
    qDebug() << workman.getWorkSpace();
    qDebug() << "making icons";
    Razordeskmanagerlegacy deskman(workman.getWorkSpace());
    deskman.updateIconList();


    return app.exec();
}

