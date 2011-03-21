#include "iconscene.h"

IconScene::IconScene(const QString & directory, QObject * parent)
    : QGraphicsScene(parent)
{
    addText(directory);
}
