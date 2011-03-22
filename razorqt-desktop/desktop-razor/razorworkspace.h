#ifndef RAZORWORKSPACE_H
#define RAZORWORKSPACE_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <razorqt/xfitman.h>

/**
 * @file razorworkspace.h
 * @brief declares class Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */

class WorkspaceConfig;
class ReadSettings;

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
    void mouseReleased(QMouseEvent*);
    void mouseWheeled(int);

protected:
    void mouseMoveEvent(QMouseEvent* _ev);
    void mousePressEvent(QMouseEvent* _ev);
    void mouseReleaseEvent(QMouseEvent* _ev);
    void wheelEvent(QWheelEvent* _ev);

private:
    QGraphicsScene * m_scene;
    ReadSettings * m_config;
    int m_screen;

private slots:
    void workspaceResized(int screen);
};




#endif
