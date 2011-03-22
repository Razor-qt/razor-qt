#ifndef DESKTOPICON_H
#define DESKTOPICON_H

#include <QGraphicsLayoutItem>
#include <QGraphicsTextItem>
#include <razorqt/xdgdesktopfile.h>


class IconViewLabel : public QGraphicsTextItem
{
    Q_OBJECT
    
public:
    IconViewLabel(const QString & text, QGraphicsItem * parent = 0);
};


class IconBase : public QGraphicsItem
{
public:
    IconBase(QGraphicsItem * parent=0);

    QRectF boundingRect() const;
    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);

    void setIcon(const QIcon & icon);
    void setText(const QString & text);

    static int iconWidth() { return 80; }
    static int iconHeight() { return 80; }
    
protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);

    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent * event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);

private:
    QPixmap m_pm; // std
    QPixmap m_pm_hi; // highlight
    bool m_highlight;
    QString m_text;

    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    virtual void launchApp() = 0;
};


/** \brief Icon for the XDG desktop file. It uses XdgDesktopFile handler
from librazorqt.
 */
class DesktopIcon : public IconBase
{
    
public:
    DesktopIcon(XdgDesktopFile * xdg,
                QGraphicsItem * parent = 0
               );
    ~DesktopIcon();

private:
    XdgDesktopFile * m_xdg;

    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    void launchApp();

};

/** \brief Icon for standard file. It uses QDesktopServices to launch the
"open with" application by file mime type.
 */
class FileIcon : public IconBase
{
    
public:
    FileIcon(const QString & file,
             QGraphicsItem * parent = 0
            );

private:
    QString m_file;

    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    void launchApp();

};

#endif // DESKTOPICON_H
