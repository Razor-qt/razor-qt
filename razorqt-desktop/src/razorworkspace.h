#ifndef RAZORWORKSPACE_H
#define RAZORWORKSPACE_H

#include "defs.h"
#include <razorqt/xfitman.h>
/**
 * @file razorworkspace.h
 * @brief declares class Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */



/**
 * @brief This one is the actual workspace
 */

class RazorWorkSpace : public QLabel
{
    Q_OBJECT
public:
    RazorWorkSpace(QWidget* _parent = 0, Qt::WindowFlags _f = 0);
    ~RazorWorkSpace();
signals:
    void mouseReleased(QMouseEvent*);
    void mouseWheeled(int);

protected:
    void mouseMoveEvent(QMouseEvent* _ev);
    void mousePressEvent(QMouseEvent* _ev);
    void mouseReleaseEvent(QMouseEvent* _ev);
    void wheelEvent(QWheelEvent* _ev);
};

/**
 * @brief this class does all the managing, like looking in the settings for a wallpaper
 */

class RazorWorkSpaceManager : public QObject
{
    Q_OBJECT
public:

    RazorWorkSpaceManager(QObject* parent = 0);
    ~RazorWorkSpaceManager();
    RazorWorkSpace* getWorkSpace()
    {
        return workSpace;
    }
public slots:
    void switchDesktop(int);
    void mouseClicked(QMouseEvent*);
private:
    RazorWorkSpace* workSpace;

};


#endif
