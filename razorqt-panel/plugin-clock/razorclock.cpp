#ifndef RAZORCLOCK_CPP
#define RAZORCLOCK_CPP

#include "razorclock.h"
#include "razor.h"
#include "razorbar.h"
#include "razorqt/readsettings.h"

/**
 * @file razorclock.cpp
 * @brief implements Razorclock and Razorclockgui
 * @author Christopher "VdoP" Regali
 */
RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorClock * ret = new RazorClock(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}

/**
 * @brief constructor
 */
RazorClock::RazorClock(RazorBar * panel, QWidget * parent, const QString & name): RazorPlugin(panel, parent, name)
{
    m_configId = name;
    cfg = new ReadSettings("clock");
    cfg->settings()->beginGroup(name);
    clockFormat = cfg->settings()->value("format", "hh:mm").toString();
    cfg->settings()->endGroup();
    
    qDebug() << "Razorclock loading";
    //gui machen
    gui = new QLabel(this);
    gui->setAlignment(Qt::AlignCenter);
    mainLayout()->addWidget(gui);
    // ensure wi will not have moving widgets in the panel waiting for timer shot
    updateTime();

    // reduce required space
    if (panel->topbottom())
    {
        setMaximumHeight(panel->height());
        gui->setMaximumHeight(panel->height());
    }
    else
    {
        setMaximumWidth(panel->width());
        gui->setMaximumWidth(panel->width());
    }    
    // this block is a fixup mainly for vertical panels where is the space limited
    // for most masks. It tries to calculate maximal but still fully displayed
    // bounding box for all datetime clockFormat
    QFont font(gui->font());
    QString temp(QDateTime::currentDateTime().toString(clockFormat));
    temp = temp.replace("<br>", "\n", Qt::CaseInsensitive);
    temp = temp.replace("<br/>", "\n", Qt::CaseInsensitive);
    while (true)
    {
        QFontMetrics fm(font);
        QRect bbox = fm.boundingRect(gui->geometry(),
                                     Qt::AlignCenter,
                                     temp);
        qDebug() << bbox.width() << gui->width() << width() << bbox.height() << gui->height() << height() << temp;
        if (bbox.width() >= gui->width() || bbox.height() >= gui->height())
        {
            qDebug() << "RazorClock: font is too big for widget. resizing. current:" << font;
            font.setPointSize(font.pointSize() - 1);
            gui->setFont(font);
            continue;
        }
        break;
    }

    //timer machen
    clocktimer = new QTimer(this);
    //timer mit uhrupdate verbinden
    connect (clocktimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    //timer starten
    clocktimer->start(1000);
}

/**
 * @brief updates the time
 * Color and font settings can be configured in Qt CSS
 */
void RazorClock::updateTime()
{
    gui->setText(QDateTime::currentDateTime().toString(clockFormat));
}

/**
 * @brief destructor
 */
RazorClock::~RazorClock()
{
}

#endif
