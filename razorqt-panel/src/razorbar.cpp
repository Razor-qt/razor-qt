#ifndef RAZORBAR_CPP
#define RAZORBAR_CPP

#include "razorbar.h"
#include "razor.h"
#include <razorqt/readsettings.h>

/**
 * @file razorbar.cpp
 * @brief implements Razorbar, the bar-menu on the bottom
 * @author Christopher "VdoP" Regali
 */


/**
 * @brief constructor
 */
RazorBar::RazorBar(const QString & configId)
{
    m_configId = configId;
    cfg = new ReadSettings("panel");
    QSettings * s = cfg->settings();
    s->beginGroup(m_configId);
    size = s->value("size", 32).toInt();
    m_position = (Position)(s->value("position", 0).toInt());

    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
    // this enables to view tooltips if they are set in the subwidgets
    setAttribute(Qt::WA_AlwaysShowToolTips);

    qDebug() << "Razorbar: initializing.." << configId;
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    // how to align plugins...
    if (topbottom())
        Layout = new QHBoxLayout();
    else
        Layout = new QVBoxLayout();
    Layout->setSpacing(1);
    Layout->setMargin(1);
    setLayout(Layout);

    if (topbottom())
        setFixedHeight(size);
    else
        setFixedWidth(size);

    // width handling - current approach:
    // if there is only one physicall screen used - all size width
    // if there are more screens/monitors only the main monitor will contain
    // the panel (full width). It prevents over-monitors-broken task manager
    // TODO: it canm be configured in the theme of course. But no high priority.
    QDesktopWidget * dw = QApplication::desktop();
    if (dw->screenCount() == 1)
    {
        //! \todo TODO/FIXME - align (left, right, center) and something like KDE3 panel "arrow"
        sizeLimit = s->value("size_limit", topbottom() ? dw->width() : dw->height()).toInt();
        topbottom() ? setFixedWidth(sizeLimit)
                    : setFixedHeight(sizeLimit);
        if (m_position == Bottom)
            move(dw->screenGeometry(-1).x(), QApplication::desktop()->height() - height());
        else
            move(dw->screenGeometry(-1).x(), 0);
    }
    else
    {
        sizeLimit = s->value("size_limit", topbottom() ? dw->screenGeometry(-1).width()
                                                       : dw->screenGeometry(-1).height()).toInt();
        topbottom() ? setFixedWidth(sizeLimit)
                    : setFixedHeight(sizeLimit);
        if (m_position == Left)
            move(dw->screenGeometry(-1).x(), 0);
        else
            move(dw->screenGeometry(-1).x()+dw->screenGeometry(-1).width()-size, 0);
    }
    qDebug() << "Panel size limit:" << sizeLimit;

    s->endGroup();
    show();
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

void RazorBar::pluginSizeChanged()
{
    qDebug() << sender() << " asked for relayout";
    qDebug() << "    panel items=" << barItems;
    int widthAvail = width();
    qDebug() << "    panel width=" << widthAvail;
    QList<RazorPlugin*> wToGrowth;

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
            w->setWidth(newDimension);
            continue;
        }
        qDebug() << w << "new dynamic";
        wToGrowth.append(w);
    }

    if (wToGrowth.size() == 0)
        return;

    double res = widthAvail / wToGrowth.size();
    foreach(RazorPlugin * w, wToGrowth)
    {
        qDebug() << w << "expanding:" << res;
        w->setWidth(res);
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
