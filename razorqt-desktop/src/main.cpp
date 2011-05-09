#include <QApplication>
#include <QLibrary>
#include <QtDebug>

#include <razorqt/readsettings.h>
#include "desktopplugin.h"



int main (int argc, char* argv[])
{
    //TODO make singleton QApplication that does all this shit, then make razorsettings and stylesettings available and fix all the other stuff with it

    QApplication app(argc,argv);
    
    ReadSettings s("razor");
    ReadTheme theme(s.settings()->value("theme", "").toString());
    ReadSettings config("desktop");

    app.setStyleSheet(theme.qss());
    
    QString configId(config.settings()->value("desktop", "razor").toString());
    QString libraryFileName = QString(DESKTOP_PLUGIN_DIR) + "libdesktop-" + configId + ".so";

    qDebug() << "RazorDesktop: try to load " << libraryFileName;

    QLibrary * lib = new QLibrary(libraryFileName);
    PluginInitFunction initFunc = (PluginInitFunction) lib->resolve("init");
    if (!initFunc)
    {
        qDebug() << lib->errorString();
        delete lib;
        return 0;
    }

    DesktopPlugin * plugin = initFunc(configId, &config, &theme);
    Q_ASSERT(plugin);

    if (plugin)
    {
        //lib->setParent(plugin);
        qDebug() << "    * Plugin loaded.";
        qDebug() << plugin->info();
    }

    return app.exec();
}

