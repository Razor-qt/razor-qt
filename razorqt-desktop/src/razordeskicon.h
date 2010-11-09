#ifndef RAZORDESKICON_H
#define RAZORDESKICON_H

#include "defs.h"

/**
 * @file razordeskicon.h
 * @author Christopher "VdoP" Regali
 * @brief declares the classes Razordeskicon and Razordeskicondata
 */

class Razordeskicon;
/**
 * @brief stores the data used for the desktop icon
 */

class Razordeskicondata : public QAction
{
    Q_OBJECT
public:
    Razordeskicondata(const QIcon& _icon, const QString& _text,QString _commentTT,  QPoint _position, QWidget* _parent);
    ~Razordeskicondata();
    QPoint getPosition();
    QString getTT()
    {
        return commentTT;
    }
    Window  getWin();
    void    setPos(QPoint _npos);
public slots:
    void updatePosition(QPoint);
    void doAction();
signals:
    void moved(QPoint);
private:
    QString commentTT;
    QPoint* position;
    Razordeskicon* gui;
};


/**
 * @brief this class represents a single desktop-icon - GUI
 */
class Razordeskicon : public QToolButton
{
    Q_OBJECT
public:
    Razordeskicon(QWidget* parent = 0);
    Razordeskicon(Razordeskicondata* _data, QWidget* _parent);
    ~Razordeskicon();

protected:
    void  mousePressEvent(QMouseEvent* _event);
    void  mouseMoveEvent(QMouseEvent* _event);
    void  mouseReleaseEvent(QMouseEvent* _event);
signals:
    void  moved(QPoint);
private:
    bool moveMe;
    bool movedMe;
    bool firstGrab;
    QPoint firstPos;
    Razordeskicondata* data;
};

#endif

