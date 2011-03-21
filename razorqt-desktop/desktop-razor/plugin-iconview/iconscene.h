#ifndef ICONSCENE_H
#define ICONSCENE_H

#include <QGraphicsScene>


class IconScene : public QGraphicsScene
{
public:
    IconScene(const QString & directory, QObject * parent = 0);
};

#endif
