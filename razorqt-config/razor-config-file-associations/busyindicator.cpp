/*
 * File:   BusyIndicator.cpp
 * Author: christian
 *
 * Created on 22. maj 2013, 08:20
 */
#include <math.h>
#include <sys/param.h>

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QRect>
#include <QResizeEvent>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include <QWidget>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include "busyindicator.h"

#define numberOfDots 15
#define radius 30
#define dotRadius 5
 
BusyIndicator::BusyIndicator(QWidget *busyWidget) : QGraphicsView(busyWidget)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *scene = new QGraphicsScene(this) ;
    setScene(scene);
    setAlignment(Qt::AlignCenter);
    for (int i = 0; i < numberOfDots; i++)
    {
        QGraphicsEllipseItem *dot = new QGraphicsEllipseItem(-dotRadius, -dotRadius, dotRadius, dotRadius);
        dot->setOpacity(1.0*(numberOfDots - i)/numberOfDots);
        m_Dots << dot;
        scene->addItem(dot);
    }
     
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(100);
    startTime = QDateTime::currentMSecsSinceEpoch();
    pendingResize = true;
}

void BusyIndicator::resizeEvent(QResizeEvent* event)
{
    pendingResize = true; 
    event->accept();
}

void BusyIndicator::moveDot(QGraphicsEllipseItem *dot, float angle)
{
    QPointF pos = center + QPointF(radius*cos(angle), radius*sin(angle)) - QPointF(dotRadius, dotRadius);
    dot->setPos(pos);
}


void BusyIndicator::update()
{
    if (pendingResize)
    {
        setSceneRect(0, 0, size().width(), size().height());
        center = QPointF(size().width()/2, size().height()/2);
        pendingResize = false;
    }
    long timeGone = QDateTime::currentMSecsSinceEpoch() - startTime;
    float angle = M_PI*timeGone/1000; // 2 seconds -> full circle
    for (int i = 0; i < numberOfDots; i++)
    {
        moveDot(m_Dots[i], angle);
        angle = angle - M_PI/10; // 20 dots -> full circle
    }
}


BusyIndicator::~BusyIndicator()
{
}
