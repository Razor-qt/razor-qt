/* 
 * File:   BusyIndicator.h
 * Author: christian
 *
 * Created on 22. maj 2013, 08:20
 */

#ifndef _BUSYINDICATOR_H
#define	_BUSYINDICATOR_H

#include <QList>
#include <QFutureWatcher>
#include <QGraphicsView>

class QResizeEvent;
class QGraphicsEllipseItem;

class BusyIndicator : public QGraphicsView
{
    Q_OBJECT
public:
    BusyIndicator(QWidget* busyWidget = 0);
    virtual ~BusyIndicator();

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void update();
 
private:
    void moveDot(QGraphicsEllipseItem *dot, float angle);
   
    QPointF center;   

    long startTime;
    bool pendingResize;

    QList<QGraphicsEllipseItem*> m_Dots;
};

#endif	/* _BUSYINDICATOR_H */
