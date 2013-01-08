/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "razorsysstat.h"
#include "razorsysstatutils.h"

#include <sysstat/cpustat.hpp>
#include <sysstat/memstat.hpp>
#include <sysstat/netstat.hpp>

#include <QtCore/QTimer>
#include <QtCore/qmath.h>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorSysStat)

RazorSysStat::RazorSysStat(const RazorPanelPluginStartInfo *startInfo, QWidget *parent):
    RazorPanelPlugin(startInfo, parent),
    mFakeTitle(new RazorSysStatTitle(this)),
    mContent(new RazorSysStatContent(panel(), this))
{
    setObjectName("SysStat");

    QWidget *border = new QWidget(this);

    QVBoxLayout *borderLayout = new QVBoxLayout(border);
    borderLayout->setContentsMargins(0, 0, 0, 0);
    borderLayout->setSpacing(0);
    borderLayout->addWidget(mContent);
    borderLayout->setStretchFactor(mContent, 1);

    addWidget(border);

    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);

    mContent->setMinimumSize(2, 2);

    // qproperty of font type doesn't work with qss, so fake QLabel is used instead
    connect(mFakeTitle, SIGNAL(fontChanged(QFont)), mContent, SLOT(setTitleFont(QFont)));

    // has to be postponed to update the size first
    QTimer::singleShot(0, this, SLOT(lateInit()));
}

RazorSysStat::~RazorSysStat()
{
}

void RazorSysStat::lateInit()
{
    settingsChanged();
    mContent->setTitleFont(mFakeTitle->font());
}

void RazorSysStat::showConfigureDialog()
{
    RazorSysStatConfiguration *confWindow = this->findChild<RazorSysStatConfiguration*>("SysStatConfigurationWindow");

    if (!confWindow)
        confWindow = new RazorSysStatConfiguration(settings(), this);

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

void RazorSysStat::settingsChanged()
{
    mContent->updateSettings(settings());
}

RazorSysStatTitle::RazorSysStatTitle(QWidget *parent):
    QLabel(parent)
{

}

RazorSysStatTitle::~RazorSysStatTitle()
{

}

bool RazorSysStatTitle::event(QEvent *e)
{
    if (e->type() == QEvent::FontChange)
        emit fontChanged(font());

    return QLabel::event(e);
}

RazorSysStatContent::RazorSysStatContent(RazorPanel *panel, QWidget *parent):
    QWidget(parent),
    mPanel(panel),
    mStat(NULL),
    mUpdateInterval(0),
    mMinimalSize(0),
    mTitleFontPixelHeight(0),
    mUseThemeColours(true),
    mHistoryOffset(0)
{
    setObjectName("SysStat_Graph");

    connect(mPanel, SIGNAL(layoutDirectionChanged(QBoxLayout::Direction)), SLOT(reset()));
}

RazorSysStatContent::~RazorSysStatContent()
{
}


// I don't like macros very much, but writing dozen similar functions is much much worse.

#undef QSS_GET_COLOUR
#define QSS_GET_COLOUR(GETNAME) \
QColor RazorSysStatContent::GETNAME##Colour(void) const \
{ \
    return mThemeColours.GETNAME##Colour; \
}

#undef QSS_COLOUR
#define QSS_COLOUR(GETNAME, SETNAME) \
QSS_GET_COLOUR(GETNAME) \
void RazorSysStatContent::SETNAME##Colour(QColor value) \
{ \
    mThemeColours.GETNAME##Colour = value; \
    if (mUseThemeColours) \
        mColours.GETNAME##Colour = mThemeColours.GETNAME##Colour; \
}

#undef QSS_NET_COLOUR
#define QSS_NET_COLOUR(GETNAME, SETNAME) \
QSS_GET_COLOUR(GETNAME) \
void RazorSysStatContent::SETNAME##Colour(QColor value) \
{ \
    mThemeColours.GETNAME##Colour = value; \
    if (mUseThemeColours) \
    { \
        mColours.GETNAME##Colour = mThemeColours.GETNAME##Colour; \
        mixNetColours(); \
    } \
}

QSS_COLOUR(grid,      setGrid)
QSS_COLOUR(title,     setTitle)
QSS_COLOUR(cpuSystem, setCpuSystem)
QSS_COLOUR(cpuUser,   setCpuUser)
QSS_COLOUR(cpuNice,   setCpuNice)
QSS_COLOUR(cpuOther,  setCpuOther)
QSS_COLOUR(frequency, setFrequency)
QSS_COLOUR(memApps,   setMemApps)
QSS_COLOUR(memBuffers,setMemBuffers)
QSS_COLOUR(memCached, setMemCached)
QSS_COLOUR(swapUsed,  setSwapUsed)

QSS_NET_COLOUR(netReceived,    setNetReceived)
QSS_NET_COLOUR(netTransmitted, setNetTransmitted)

#undef QSS_NET_COLOUR
#undef QSS_COLOUR
#undef QSS_GET_COLOUR

void RazorSysStatContent::mixNetColours(void)
{
    QColor netReceivedColour_hsv = mColours.netReceivedColour.toHsv();
    QColor netTransmittedColour_hsv = mColours.netTransmittedColour.toHsv();
    qreal hue = (netReceivedColour_hsv.hueF() + netTransmittedColour_hsv.hueF()) / 2;
    if (qAbs(netReceivedColour_hsv.hueF() - netTransmittedColour_hsv.hueF()) > 0.5)
        hue += 0.5;
    mNetBothColour.setHsvF(
        hue,
        (netReceivedColour_hsv.saturationF() + netTransmittedColour_hsv.saturationF()) / 2,
        (netReceivedColour_hsv.valueF()      + netTransmittedColour_hsv.valueF()     ) / 2 );
}

void RazorSysStatContent::setTitleFont(QFont value)
{
    mTitleFont = value;
    updateTitleFontPixelHeight();

    update();
}

void RazorSysStatContent::updateTitleFontPixelHeight(void)
{
    if (mTitleLabel.isEmpty())
        mTitleFontPixelHeight = 0;
    else
    {
        QFontMetrics fm(mTitleFont);
        mTitleFontPixelHeight = fm.height() - 1;
    }
}

void RazorSysStatContent::updateSettings(const QSettings &settings)
{
    double old_updateInterval = mUpdateInterval;
    int old_minimalSize = mMinimalSize;
    QString old_dataType = mDataType;
    QString old_dataSource = mDataSource;
    bool old_useFrequency = mUseFrequency;
    bool old_logarithmicScale = mLogarithmicScale;
    int old_logScaleSteps = mLogScaleSteps;

    mUseThemeColours = settings.value("graph/useThemeColours", true).toBool();
    mUpdateInterval = settings.value("graph/updateInterval", 1.0).toDouble();
    mMinimalSize = settings.value("graph/minimalSize", 30).toInt();

    mGridLines = settings.value("grid/lines", 1).toInt();

    mTitleLabel = settings.value("title/label", QString()).toString();

    mDataType = settings.value("data/type", QString("CPU")).toString();

    mDataSource = settings.value("data/source", QString()).toString();

    mUseFrequency = settings.value("cpu/useFrequency", true).toBool();

    mNetMaximumSpeed = PluginSysStat::netSpeedFromString(settings.value("net/maximumSpeed", "1 MB/s").toString());
    mLogarithmicScale = settings.value("net/logarithmicScale", true).toBool();

    mLogScaleSteps = settings.value("net/logarithmicScaleSteps", 4).toInt();
    mLogScaleMax = static_cast<qreal>(static_cast<int64_t>(1) << mLogScaleSteps);

    mNetRealMaximumSpeed = static_cast<qreal>(static_cast<int64_t>(1) << mNetMaximumSpeed);


    mSettingsColours.gridColour = QColor(settings.value("grid/colour", "#c0c0c0").toString());

    mSettingsColours.titleColour = QColor(settings.value("title/colour", "#ffffff").toString());

    mSettingsColours.cpuSystemColour = QColor(settings.value("cpu/systemColour",    "#800000").toString());
    mSettingsColours.cpuUserColour   = QColor(settings.value("cpu/userColour",      "#000080").toString());
    mSettingsColours.cpuNiceColour   = QColor(settings.value("cpu/niceColour",      "#008000").toString());
    mSettingsColours.cpuOtherColour  = QColor(settings.value("cpu/otherColour",     "#808000").toString());
    mSettingsColours.frequencyColour = QColor(settings.value("cpu/frequencyColour", "#808080").toString());

    mSettingsColours.memAppsColour    = QColor(settings.value("mem/appsColour",    "#000080").toString());
    mSettingsColours.memBuffersColour = QColor(settings.value("mem/buffersColour", "#008000").toString());
    mSettingsColours.memCachedColour  = QColor(settings.value("mem/cachedColour",  "#808000").toString());
    mSettingsColours.swapUsedColour   = QColor(settings.value("mem/swapColour",    "#800000").toString());

    mSettingsColours.netReceivedColour    = QColor(settings.value("net/receivedColour",    "#000080").toString());
    mSettingsColours.netTransmittedColour = QColor(settings.value("net/transmittedColour", "#808000").toString());


    if (mUseThemeColours)
        mColours = mThemeColours;
    else
        mColours = mSettingsColours;

    mixNetColours();

    updateTitleFontPixelHeight();


    bool minimalSizeChanged      = old_minimalSize      != mMinimalSize;
    bool updateIntervalChanged   = old_updateInterval   != mUpdateInterval;
    bool dataTypeChanged         = old_dataType         != mDataType;
    bool dataSourceChanged       = old_dataSource       != mDataSource;
    bool useFrequencyChanged     = old_useFrequency     != mUseFrequency;
    bool logScaleStepsChanged    = old_logScaleSteps    != mLogScaleSteps;
    bool logarithmicScaleChanged = old_logarithmicScale != mLogarithmicScale;

    bool needReconnecting    = dataTypeChanged || dataSourceChanged || useFrequencyChanged;
    bool needTimerRestarting = needReconnecting || updateIntervalChanged;
    bool needFullReset       = needTimerRestarting || minimalSizeChanged || logScaleStepsChanged || logarithmicScaleChanged;


    if (mStat)
    {
        if (needTimerRestarting)
            mStat->stopUpdating();

        if (needReconnecting)
            mStat->disconnect(this);
    }

    if (dataTypeChanged)
    {
        if (mStat)
            mStat->deleteLater();

        if (mDataType == "CPU")
            mStat = new SysStat::CpuStat(this);
        else if (mDataType == "Memory")
            mStat = new SysStat::MemStat(this);
        else if (mDataType == "Network")
            mStat = new SysStat::NetStat(this);
    }

    if (mStat)
    {
        if (needReconnecting)
        {
            if (mDataType == "CPU")
            {
                if (mUseFrequency)
                {
                    qobject_cast<SysStat::CpuStat*>(mStat)->setMonitoring(SysStat::CpuStat::LoadAndFrequency);
                    connect(qobject_cast<SysStat::CpuStat*>(mStat), SIGNAL(update(float, float, float, float, float, uint)), this, SLOT(cpuUpdate(float, float, float, float, float, uint)));
                }
                else
                {
                    qobject_cast<SysStat::CpuStat*>(mStat)->setMonitoring(SysStat::CpuStat::LoadOnly);
                    connect(qobject_cast<SysStat::CpuStat*>(mStat), SIGNAL(update(float, float, float, float)), this, SLOT(cpuUpdate(float, float, float, float)));
                }
            }
            else if (mDataType == "Memory")
            {
                if (mDataSource == "memory")
                    connect(qobject_cast<SysStat::MemStat*>(mStat), SIGNAL(memoryUpdate(float, float, float)), this, SLOT(memoryUpdate(float, float, float)));
                else
                    connect(qobject_cast<SysStat::MemStat*>(mStat), SIGNAL(swapUpdate(float)), this, SLOT(swapUpdate(float)));
            }
            else if (mDataType == "Network")
            {
                connect(qobject_cast<SysStat::NetStat*>(mStat), SIGNAL(update(unsigned, unsigned)), this, SLOT(networkUpdate(unsigned, unsigned)));
            }

            mStat->setMonitoredSource(mDataSource);
        }

        if (needTimerRestarting)
            mStat->setUpdateInterval(static_cast<int>(mUpdateInterval * 1000.0));
    }

    if (needFullReset)
        reset();
    else
        update();
}

void RazorSysStatContent::resizeEvent(QResizeEvent * /*event*/)
{
    reset();
}

void RazorSysStatContent::reset(void)
{
    setMinimumSize(mPanel->isHorizontal() ? mMinimalSize : 2, mPanel->isHorizontal() ? 2 : mMinimalSize);

    mHistoryOffset = 0;
    mHistoryImage = QImage(width(), 100, QImage::Format_ARGB32);
#if QT_VERSION < 0x040800
    mHistoryImage.fill(QColor(Qt::transparent).rgba());
#else
    mHistoryImage.fill(Qt::transparent);
#endif

    update();
}

template <typename T>
T clamp(const T &value, const T &min, const T &max)
{
    return qMin(qMax(value, min), max);
}

// QPainter.drawLine with pen set to Qt::transparent doesn't clear anything
void RazorSysStatContent::clearLine(void)
{
    QRgb bg = QColor(Qt::transparent).rgba();
    for (int i = 0; i < 100; ++i)
        reinterpret_cast<QRgb*>(mHistoryImage.scanLine(i))[mHistoryOffset] = bg;
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other, float frequencyRate, uint)
{
    int y_system = clamp(static_cast<int>(system * 100.0 * frequencyRate)           , 0, 99);
    int y_user   = clamp(static_cast<int>(user   * 100.0 * frequencyRate) + y_system, 0, 99);
    int y_nice   = clamp(static_cast<int>(nice   * 100.0 * frequencyRate) + y_user  , 0, 99);
    int y_other  = clamp(static_cast<int>(other  * 100.0 * frequencyRate) + y_nice  , 0, 99);
    int y_freq   = clamp(static_cast<int>(         100.0 * frequencyRate)           , 0, 99);

    clearLine();
    QPainter painter(&mHistoryImage);
    if (y_system != 0)
    {
        painter.setPen(mColours.cpuSystemColour);
        painter.drawLine(mHistoryOffset, y_system, mHistoryOffset, 0);
    }
    if (y_user != y_system)
    {
        painter.setPen(mColours.cpuUserColour);
        painter.drawLine(mHistoryOffset, y_user, mHistoryOffset, y_system);
    }
    if (y_nice != y_user)
    {
        painter.setPen(mColours.cpuNiceColour);
        painter.drawLine(mHistoryOffset, y_nice, mHistoryOffset, y_user);
    }
    if (y_other != y_nice)
    {
        painter.setPen(mColours.cpuOtherColour);
        painter.drawLine(mHistoryOffset, y_other, mHistoryOffset, y_nice);
    }
    if (y_freq != y_other)
    {
        painter.setPen(mColours.frequencyColour);
        painter.drawLine(mHistoryOffset, y_freq, mHistoryOffset, y_other);
    }

    mHistoryOffset = (mHistoryOffset + 1) % width();

    update(0, mTitleFontPixelHeight, width(), height() - mTitleFontPixelHeight);
}

void RazorSysStatContent::cpuUpdate(float user, float nice, float system, float other)
{
    int y_system = clamp(static_cast<int>(system * 100.0)           , 0, 99);
    int y_user   = clamp(static_cast<int>(user   * 100.0) + y_system, 0, 99);
    int y_nice   = clamp(static_cast<int>(nice   * 100.0) + y_user  , 0, 99);
    int y_other  = clamp(static_cast<int>(other  * 100.0) + y_nice  , 0, 99);

    clearLine();
    QPainter painter(&mHistoryImage);
    if (y_system != 0)
    {
        painter.setPen(mColours.cpuSystemColour);
        painter.drawLine(mHistoryOffset, y_system, mHistoryOffset, 0);
    }
    if (y_user != y_system)
    {
        painter.setPen(mColours.cpuUserColour);
        painter.drawLine(mHistoryOffset, y_user, mHistoryOffset, y_system);
    }
    if (y_nice != y_user)
    {
        painter.setPen(mColours.cpuNiceColour);
        painter.drawLine(mHistoryOffset, y_nice, mHistoryOffset, y_user);
    }
    if (y_other != y_nice)
    {
        painter.setPen(mColours.cpuOtherColour);
        painter.drawLine(mHistoryOffset, y_other, mHistoryOffset, y_nice);
    }

    mHistoryOffset = (mHistoryOffset + 1) % width();

    update(0, mTitleFontPixelHeight, width(), height() - mTitleFontPixelHeight);
}

void RazorSysStatContent::memoryUpdate(float apps, float buffers, float cached)
{
    int y_apps    = clamp(static_cast<int>(apps    * 100.0)            , 0, 99);
    int y_buffers = clamp(static_cast<int>(buffers * 100.0) + y_apps   , 0, 99);
    int y_cached  = clamp(static_cast<int>(cached  * 100.0) + y_buffers, 0, 99);

    clearLine();
    QPainter painter(&mHistoryImage);
    if (y_apps != 0)
    {
        painter.setPen(mColours.memAppsColour);
        painter.drawLine(mHistoryOffset, y_apps, mHistoryOffset, 0);
    }
    if (y_buffers != y_apps)
    {
        painter.setPen(mColours.memBuffersColour);
        painter.drawLine(mHistoryOffset, y_buffers, mHistoryOffset, y_apps);
    }
    if (y_cached != y_buffers)
    {
        painter.setPen(mColours.memCachedColour);
        painter.drawLine(mHistoryOffset, y_cached, mHistoryOffset, y_buffers);
    }

    mHistoryOffset = (mHistoryOffset + 1) % width();

    update(0, mTitleFontPixelHeight, width(), height() - mTitleFontPixelHeight);
}

void RazorSysStatContent::swapUpdate(float used)
{
    int y_used = clamp(static_cast<int>(used * 100.0), 0, 99);

    clearLine();
    QPainter painter(&mHistoryImage);
    if (y_used != 0)
    {
        painter.setPen(mColours.swapUsedColour);
        painter.drawLine(mHistoryOffset, y_used, mHistoryOffset, 0);
    }

    mHistoryOffset = (mHistoryOffset + 1) % width();

    update(0, mTitleFontPixelHeight, width(), height() - mTitleFontPixelHeight);
}

void RazorSysStatContent::networkUpdate(unsigned received, unsigned transmitted)
{
    qreal min_value = qMin(qMax(static_cast<qreal>(qMin(received, transmitted)) / mNetRealMaximumSpeed, static_cast<qreal>(0.0)), static_cast<qreal>(1.0));
    qreal max_value = qMin(qMax(static_cast<qreal>(qMax(received, transmitted)) / mNetRealMaximumSpeed, static_cast<qreal>(0.0)), static_cast<qreal>(1.0));
    if (mLogarithmicScale)
    {
        min_value = qLn(min_value * (mLogScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(mLogScaleSteps);
        max_value = qLn(max_value * (mLogScaleMax - 1.0) + 1.0) / qLn(2.0) / static_cast<qreal>(mLogScaleSteps);
    }

    int y_min_value = clamp(static_cast<int>(min_value * 100.0)              , 0, 99);
    int y_max_value = clamp(static_cast<int>(max_value * 100.0) + y_min_value, 0, 99);

    clearLine();
    QPainter painter(&mHistoryImage);
    if (y_min_value != 0)
    {
        painter.setPen(mNetBothColour);
        painter.drawLine(mHistoryOffset, y_min_value, mHistoryOffset, 0);
    }
    if (y_max_value != y_min_value)
    {
        painter.setPen((received > transmitted) ? mColours.netReceivedColour : mColours.netTransmittedColour);
        painter.drawLine(mHistoryOffset, y_max_value, mHistoryOffset, y_min_value);
    }

    mHistoryOffset = (mHistoryOffset + 1) % width();

    update(0, mTitleFontPixelHeight, width(), height() - mTitleFontPixelHeight);
}

void RazorSysStatContent::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    qreal graphTop = 0;
    qreal graphHeight = height();

    bool hasTitle = !mTitleLabel.isEmpty();

    if (hasTitle)
    {
        graphTop = mTitleFontPixelHeight;
        graphHeight -= graphTop;

        if (event->region().intersects(QRect(0, 0, width(), graphTop)))
        {
            p.setPen(mColours.titleColour);
            p.setFont(mTitleFont);
            p.drawText(QRectF(0, 0, width(), graphTop), Qt::AlignHCenter | Qt::AlignVCenter, mTitleLabel);
        }
    }

    if (graphHeight < 1)
        graphHeight = 1;

    p.scale(1.0, -1.0);

    p.drawImage(QRect(0, -height(), width() - mHistoryOffset, graphHeight), mHistoryImage, QRect(mHistoryOffset, 0, width() - mHistoryOffset, 100));
    if (mHistoryOffset)
        p.drawImage(QRect(width() - mHistoryOffset, -height(), mHistoryOffset, graphHeight), mHistoryImage, QRect(0, 0, mHistoryOffset, 100));

    p.resetTransform();

    p.setRenderHint(QPainter::Antialiasing);

    p.setPen(mColours.gridColour);
    qreal w = static_cast<qreal>(width());
    if (hasTitle)
        p.drawLine(QPointF(0.0, graphTop + 0.5), QPointF(w, graphTop + 0.5)); // 0.5 looks better with antialiasing
    for (int l = 0; l < mGridLines; ++l)
    {
        qreal y = graphTop + static_cast<qreal>(l + 1) * graphHeight / (static_cast<qreal>(mGridLines + 1));
        p.drawLine(QPointF(0.0, y), QPointF(w, y));
    }
}
