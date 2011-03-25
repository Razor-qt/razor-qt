#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QGraphicsItem>

#include <razorqt/readsettings.h>
#include <desktopwidgetplugin.h>


class HelloWorld : public DesktopWidgetPlugin, public QGraphicsTextItem
{
public:
    HelloWorld(QGraphicsScene * scene, const QString & configId, ReadSettings * config);
    ~HelloWorld();

    QString info();
    QString instanceInfo();

    void setSizeAndPosition(const QPointF & position, const QSizeF & size);
    void save();
};

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H


#endif
