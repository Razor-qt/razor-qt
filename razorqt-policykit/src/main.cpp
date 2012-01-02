#include <QApplication>

#include <glib-object.h>

#include <qtxdg/xdgicon.h>
#include <razorqt/razorsettings.h>

#include "policykitagent.h"

int main(int argc, char *argv[])
{
    g_type_init();

    QApplication app(argc, argv);    
    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    app.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

    RazorPolicykit::PolicykitAgent agent;
    return app.exec();
}
