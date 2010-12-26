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
    m_align = (Alignment)(s->value("align", 1).toInt());
    m_screen = s->value("screen", -1).toInt();

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
    if (m_screen > dw->screenCount()-1)
    {
        qDebug() << "Panel cfg requests screen" << m_screen << "but system has only" << dw->screenCount();
        qDebug() << "    resetting to -1";
        m_screen = -1;
    }

    sizeLimit = s->value("size_limit", topbottom() ? dw->screenGeometry(m_screen).width()
                                                   : dw->screenGeometry(m_screen).height()).toInt();
    topbottom() ? setFixedWidth(sizeLimit)
                : setFixedHeight(sizeLimit);
    int pos;
    switch (m_align)
    {
        case TopLeft:
            pos = 0;
            break;
        case BottomRight:
            if (topbottom())
                pos = dw->screenGeometry(m_screen).width() - width();
            else
                pos = dw->screenGeometry(m_screen).height() - height();
            break;
        case Center:
            if (topbottom())
                pos = (dw->screenGeometry(m_screen).width()/2) - (width()/2);
            else
                pos = (dw->screenGeometry(m_screen).height()/2) - (height()/2);
            break;
    }
    switch (m_position)
    {
        case Left:
            move(dw->screenGeometry(m_screen).x(), pos);
            break;
        case Right:
            move(dw->screenGeometry(m_screen).x() + dw->screenGeometry(m_screen).width() - size,
                 pos);
            break;
        case Top:
            move(dw->screenGeometry(m_screen).x() + pos, 0);
            break;
        case Bottom:
            move(dw->screenGeometry(m_screen).x() + pos,
                 dw->screenGeometry(m_screen).height() - size);
            break;
    };
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

    if (topbottom())
        _widget->setFixedHeight(size);
    else
        _widget->setFixedWidth(size);
}

void RazorBar::pluginSizeChanged()
{
    qDebug() << sender() << " asked for relayout";
    qDebug() << "    panel items=" << barItems;
    int widthAvail = topbottom() ? width() : height();
    qDebug() << "    panel width=" << widthAvail;
    QList<RazorPlugin*> wToGrowth;

    foreach(RazorPlugin * w, barItems.keys())
    {
        widthAvail -= Layout->spacing();
        int newDimension = topbottom() ? w->widthForHeight(height())
                                       : w->heightForWidth(width());
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
            if (topbottom())
                w->setWidth(newDimension);
            else
                w->setHeight(newDimension);
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
        if (topbottom())
            w->setWidth(res);
        else
            w->setHeight(res);
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
