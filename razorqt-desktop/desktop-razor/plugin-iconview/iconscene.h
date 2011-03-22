#ifndef ICONSCENE_H
#define ICONSCENE_H

#include <QGraphicsScene>

class QFileSystemWatcher;
class IconViewLabel;


class IconScene : public QGraphicsScene
{
    Q_OBJECT

public:
    IconScene(const QString & directory, QObject * parent = 0);

    void setParentSize(const QSizeF & size);

private:
    QFileSystemWatcher * m_fsw;
    QSizeF m_parentSize;

private slots:
    void updateIconList();
};

#endif
