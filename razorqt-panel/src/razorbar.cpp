#ifndef RAZORBAR_CPP
#define RAZORBAR_CPP

#include "razorbar.h"
#include "razor.h"

/**
 * @file razorbar.cpp
 * @brief implements Razorbar, the bar-menu on the bottom
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor
 */
RazorBar::RazorBar()
{
    /*QDialog* bla = new QDialog(NULL,Qt::FramelessWindowHint | Qt::Dialog);
    bla->show();
    */
    qDebug() << "blobb" << Razor::getInstance().get_looknfeel()->getInt("razorbar_height");
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
    // this enables to view tooltips if they are set in the subwidgets
    setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "Razorbar: initializing.." << height();
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);
    Layout = new QHBoxLayout;
    setScaledContents(true);
    Layout->setSpacing(1);
    Layout->setMargin(1);
    setLayout(Layout);

    makeUp();
    show();
    //Razor::getInstance().get_Xfitman()->moveWindowtoDesktop(effectiveWinId(),1);
}

/**
 * @brief adds a widget to the bar
 */
void RazorBar::addWidget(RazorPlugin* _widget,int _stretch, Qt::Alignment _align)
{
    //add the new widget
    Layout->addWidget(_widget, _stretch, _align);
    //save this widget in the barItems list
    barItems[_widget] = 0; //_widget->widthForHeight(height());
    connect(_widget, SIGNAL(sizeChanged()), this, SLOT(pluginSizeChanged()));
}

/**
 * @brief makes up our style
 */
void RazorBar::makeUp()
{
    setFixedHeight(Razor::getInstance().get_looknfeel()->getInt("razorbar_height"));

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

    if (! Razor::getInstance().get_looknfeel()->getString("razorbar_background").isEmpty())
    {
        setPixmap((QPixmap)(Razor::getInstance().get_looknfeel()->getPath() +  Razor::getInstance().get_looknfeel()->getString("razorbar_background")));
        qDebug() << Razor::getInstance().get_looknfeel()->getString("razorbar_background");
    }
    move(dw->screenGeometry(-1).x(), QApplication::desktop()->height() - height());
}

void RazorBar::pluginSizeChanged()
{
    qDebug() << sender() << " asked for relayout";
    qDebug() << "    panel items=" << barItems;
    int widthAvail = width();
    qDebug() << "    panel width=" << widthAvail;
    QWidgetList wToGrowth;

    foreach(RazorPlugin * w, barItems.keys())
    {
        widthAvail -= Layout->spacing();
        int newDimension = w->widthForHeight(height());
        if (barItems[w] == newDimension)
        {
            qDebug() << w << "the same size";
            widthAvail -= barItems[w];
            continue;
        }
        if (w->sizePriority() == RazorPlugin::Static)
        {
            qDebug() << w << "new size" << newDimension;
            barItems[w] = newDimension;
            widthAvail -= newDimension;
            w->setMaximumWidth(newDimension);
            w->setMinimumWidth(newDimension);
            continue;
        }
        qDebug() << w << "new dynamic";
        wToGrowth.append(w);
    }

    if (wToGrowth.size() == 0)
        return;

    double res = widthAvail / wToGrowth.size();
    foreach(QWidget * w, wToGrowth)
    {
        qDebug() << w << "expanding:" << res;
        w->setMaximumWidth(res);
        w->setMinimumWidth(res);
    }
}

/**
 * @brief destructor
 */
RazorBar::~RazorBar()
{
    delete Layout;
}



#endif
