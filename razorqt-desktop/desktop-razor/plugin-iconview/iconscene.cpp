#include "iconscene.h"
#include "desktopicon.h"


IconScene::IconScene(const QString & directory, QObject * parent)
    : QGraphicsScene(parent)
{
    addText(directory)->setPos(10, 10);
    setBackgroundBrush(Qt::green);
    
    addItem(new DesktopIcon(QPixmap("/usr/share/icons/oxygen/32x32/apps/konqueror.png"), "Konqueror", QPointF(10, 50)));
}
