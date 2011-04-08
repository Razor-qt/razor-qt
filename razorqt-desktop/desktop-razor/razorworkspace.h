#ifndef RAZORWORKSPACE_H
#define RAZORWORKSPACE_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QAction>

#include <razorqt/xfitman.h>
#include "arrangeitem.h"


/**
 * @file razorworkspace.h
 * @brief declares class Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */

class WorkspaceConfig;
class ReadSettings;
class PowerManager;

/**
 * @brief This one is the actual workspace
 */

class RazorWorkSpace : public QGraphicsView
{
    Q_OBJECT
public:
    RazorWorkSpace(ReadSettings * config, int screen=-1, QWidget * parent=0);
    ~RazorWorkSpace();

    void setConfig(const WorkspaceConfig & bg);
signals:
    void mouseWheeled(int);

protected:
    void mouseMoveEvent(QMouseEvent* _ev);
    void mousePressEvent(QMouseEvent* _ev);
    void mouseReleaseEvent(QMouseEvent* _ev);
    void wheelEvent(QWheelEvent* _ev);

private:
    
    enum Mode {
        ModeNormal,
        ModeArrange
    };

    QGraphicsScene * m_scene;
    ReadSettings * m_config;
    int m_screen;
    Mode m_mode;

    QAction * m_actArrangeWidgets;
    
    ArrangeItem * m_arrangeRoot;
    QList<ArrangeItem*> m_arrangeList;
    
    PowerManager * m_power;

private slots:
    void workspaceResized(int screen);
    void arrangeWidgets(bool start);
};




#endif
