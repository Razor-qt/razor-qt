#include "helloworld.h"
#include <QtDebug>


EXPORT_RAZOR_DESKTOP_WIDGET_PLUGIN_CPP(HelloWorld)


HelloWorld::HelloWorld(const QString & configId, ReadSettings * config)
    : DesktopWidgetPlugin(configId, config)
{
    QSettings * s = config->settings();
    s->beginGroup(configId);
    
    setOpenExternalLinks(true);
    QString text(s->value("text", "Lorem Ipsum").toString());
    qDebug() << "HelloWorld" << text;
    setHtml(text);
    
    s->endGroup();
}

HelloWorld::~HelloWorld()
{
    QSettings * s = m_config->settings();
    s->beginGroup(m_configId);
    s->setValue("position", pos());
    s->setValue("size", boundingRect().size());
    s->endGroup();
}

    
QString HelloWorld::info()
{
    return QObject::tr("Display simple text. A debugging/sample widget.");
}

void HelloWorld::setSizeAndPosition(const QPointF & position, const QSizeF & size)
{
    qDebug() << "Moving to" << position;
    setPos(position);
}
