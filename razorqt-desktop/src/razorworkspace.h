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

class Razorworkspace : public QLabel
{
    Q_OBJECT
public:
    Razorworkspace(QWidget* _parent = 0, Qt::WindowFlags _f = 0);
    ~Razorworkspace();
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

class Razorworkspacemanager : public QObject
{
    Q_OBJECT
public:

    Razorworkspacemanager(QObject* parent = 0);
    ~Razorworkspacemanager();
    Razorworkspace* getWorkSpace()
    {
        return workSpace;
    }
public slots:
    void switchDesktop(int);
    void mouseClicked(QMouseEvent*);
private:
    Razorworkspace* workSpace;

};


#endif
