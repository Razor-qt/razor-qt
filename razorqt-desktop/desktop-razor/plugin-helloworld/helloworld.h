#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <QGraphicsItem>

#include <razorqt/readsettings.h>
#include <desktopwidgetplugin.h>


class HelloWorld : public DesktopWidgetPlugin, public QGraphicsTextItem
{
public:
    HelloWorld(const QString & configId, ReadSettings * config);
    ~HelloWorld();

    QString info();

    void setSizeAndPosition(const QPointF & position, const QSizeF & size);
};

EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_H


#endif
