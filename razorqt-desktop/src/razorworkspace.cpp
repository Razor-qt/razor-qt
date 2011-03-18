#ifndef RAZORWORKSPACE_CPP
#define RAZORWORKSPACE_CPP
/**
 * @file razorworkspace.cpp
 * @brief implements Razorworkspace and Razorworkspacemanager
 * @author Christopher "VdoP" Regali
 */
#include "razorworkspace.h"
#include "razor.h"
#include <razorqt/readsettings.h>

void RazorWorkSpace::mouseMoveEvent(QMouseEvent* _ev)
{
    QLabel::mouseMoveEvent(_ev);
}

void RazorWorkSpace::mousePressEvent(QMouseEvent* _ev)
{
    QLabel::mousePressEvent(_ev);
}

void RazorWorkSpace::mouseReleaseEvent(QMouseEvent* _ev)
{
    emit mouseReleased(_ev);
    QLabel::mouseReleaseEvent(_ev);
}

void RazorWorkSpaceManager::switchDesktop(int _change)
{
    XfitMan* xfitman = Razor::getInstance().getxfitman();
    int num_desks = xfitman->getNumDesktop();
    if (_change > 0)
    {
        if ( xfitman->getActiveDesktop()+1 < num_desks)
            xfitman->setActiveDesktop(xfitman->getActiveDesktop()+1);
        else
            xfitman->setActiveDesktop(0);
    }
    if (_change < 0)
    {
        if (xfitman->getActiveDesktop()-1 >= 0)
            xfitman->setActiveDesktop(xfitman->getActiveDesktop()-1);
        else
            xfitman->setActiveDesktop(num_desks-1);
    }
}


void RazorWorkSpace::wheelEvent(QWheelEvent* _ev)
{
    int numDegrees = _ev->delta() / 8;
    int numSteps = numDegrees / 15;
    emit mouseWheeled(-numSteps);
}





RazorWorkSpace::RazorWorkSpace(QWidget* _parent, Qt::WindowFlags _f): QLabel(_parent, _f)
{
    qDebug() << "RazorWorkSpace::RazorWorkSpace";
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setAttribute(Qt::WA_X11NetWmWindowTypeDesktop);
    setScaledContents(true);
    move(0,0);
    show();
}

RazorWorkSpace::~RazorWorkSpace()
{

}

void RazorWorkSpaceManager::mouseClicked(QMouseEvent* _event)
{
    qDebug() << "click-released the workspace";
    //maybe popup a menu or stuff - i dont know
}

RazorWorkSpaceManager::RazorWorkSpaceManager(QObject* parent)
    : QObject(parent)
{
    qDebug() << "RazorWorkSpaceManager::RazorWorkSpaceManager";
    //this may actually make the icon work on multihead
    QWidget* screen = QApplication::desktop()->screen(QApplication::desktop()->primaryScreen());
    workSpace = new RazorWorkSpace(screen);

    connect(workSpace, SIGNAL(mouseReleased(QMouseEvent*)),this, SLOT(mouseClicked(QMouseEvent*)));
    connect(workSpace, SIGNAL(mouseWheeled(int)), this, SLOT(switchDesktop(int)));


    workSpace->resize(screen->width(),screen->height());



    //now we got the desktop we need to determine if the user wants a defined picture there
    QString finalPixmap = Razor::getInstance().razorsettings()->settings()->value("background_image", "").toString();
    if (finalPixmap != "internal" && finalPixmap != "")
    {
        qDebug() << "Pixmap-custom ok: " << finalPixmap;
    }
    else //now we want to use the system default - we still need to find that one out though
    {
        finalPixmap = Razor::getInstance().themesettings()->desktopBackground();
        qDebug() << "trying to get system-defaults" << finalPixmap;
    }

    //now try to set it - if its not set we assume the theme stylesheet takes care of it some way (like a gradient or stuff)
    if (finalPixmap != "")
        workSpace->setPixmap(finalPixmap);


}

RazorWorkSpaceManager::~RazorWorkSpaceManager()
{
    delete workSpace;
}




#endif
