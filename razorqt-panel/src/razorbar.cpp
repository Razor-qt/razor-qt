#ifndef RAZORBAR_CPP
#define RAZORBAR_CPP

#include "razor.h"
#include "razorbar.h"
/**
 * @file razorbar.cpp
 * @brief implements Razorbar, the bar-menu on the bottom
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor
 */
Razorbar::Razorbar()
{
    /*QDialog* bla = new QDialog(NULL,Qt::FramelessWindowHint | Qt::Dialog);
    bla->show();
    */
    qDebug() << "blobb" << Razor::getInstance().get_looknfeel()->getValue("razorbar_height").toInt();
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
	// this enables to view tooltips if they are set in the subwidgets
	setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "Razorbar: initializing.." << height();
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    Layout = new QHBoxLayout;
    setScaledContents(true);
    Layout->setSpacing(1);
    Layout->setMargin(1);
    makeUp();
    show();
    //Razor::getInstance().get_Xfitman()->moveWindowtoDesktop(effectiveWinId(),1);
}

/**
 * @brief adds a widget to the bar
 */
void Razorbar::addWidget(QWidget* _widget,int _stretch, Qt::Alignment _align)
{
    //add the new widget
    Layout->addWidget(_widget, _stretch, _align);
    // DEBUG: _widget->setStyleSheet("background-color: red");
    //_widget->setFixedSize(height()-1, height()-1);
    // set the new / old layout
    setLayout(Layout);
    //save this widget in the barItems list
    barItems.append(_widget);

}

/**
 * @brief makes up our style
 */
void Razorbar::makeUp()
{
    setFixedHeight(Razor::getInstance().get_looknfeel()->getValue("razorbar_height").toInt());

    // width handling - current approach:
    // if there is only one physicall screen used - all size width
    // if there are more screens/monitors only the main monitor will contain
    // the panel (full width). It prevents over-monitors-broken task manager
    // TODO: it canm be configured in the theme of course. But no high priority.
    QDesktopWidget * dw = QApplication::desktop();
    if (dw->screenCount() == 1)
        setFixedWidth(dw->width());
    else
        setFixedWidth(dw->screenGeometry(-1).width());

    if (! Razor::getInstance().get_looknfeel()->getValue("razorbar_background").isEmpty())

    {
        setPixmap((QPixmap)(Razor::getInstance().get_looknfeel()->getPath() +  Razor::getInstance().get_looknfeel()->getValue("razorbar_background")));
        qDebug() << Razor::getInstance().get_looknfeel()->getValue("razorbar_background");
    }
    move(dw->screenGeometry(-1).x(), QApplication::desktop()->height() - height());
}


/**
 * @brief destructor
 */
Razorbar::~Razorbar()
{
    delete Layout;
}



#endif
