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

    bool blockGlobalMenu();
    QString dir() { return m_directory; }
    void setDir(const QString & directory);

private:
    QString m_directory;
    QFileSystemWatcher * m_fsw;
    QSizeF m_parentSize;

    void setDirImpl(const QString & directory, bool repaint=false);

private slots:
    void updateIconList();
};

#endif
