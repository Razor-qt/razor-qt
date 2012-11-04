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

#ifndef RAZORPANELSYSSTAT_H
#define RAZORPANELSYSSTAT_H

#include "../panel/razorpanelplugin.h"
#include "razorsysstatconfiguration.h"


class RazorSysStatContent;

namespace SysStat {
    class BaseStat;
}

class RazorSysStat : public RazorPanelPlugin
{
    Q_OBJECT
public:
    RazorSysStat(const RazorPanelPluginStartInfo* startInfo, QWidget *parent = NULL);
    ~RazorSysStat();

    virtual RazorPanelPlugin::Flags flags() const { return PreferRightAlignment | HaveConfigDialog; }

protected slots:
    virtual void settingsChanged();
    virtual void showConfigureDialog();

private:
    RazorSysStatContent *mContent;
};

class RazorSysStatContent : public QWidget
{
    Q_OBJECT
public:
    RazorSysStatContent(RazorSysStat &plugin_, QWidget *parent = NULL);
    ~RazorSysStatContent();

    void updateSettings(const QSettings&);

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);

protected slots:
    void cpuUpdate(float user, float nice, float system, float other, float frequencyRate, uint frequency);
    void cpuUpdate(float user, float nice, float system, float other);
    void memoryUpdate(float apps, float buffers, float cached);
    void swapUpdate(float used);
    void networkUpdate(unsigned received, unsigned transmitted);

private:
    RazorSysStat &plugin;

    SysStat::BaseStat *stat;

    double updateInterval;
    int historyLength;

    int gridLines;
    QColor gridColour;

    QString titleLabel;
    QFont titleFont;
    int titleFontPixelHeight;
    QColor titleColour;

    QString dataType;

    QString dataSource;

    QColor cpuSystemColour;
    QColor cpuUserColour;
    QColor cpuNiceColour;
    QColor cpuOtherColour;
    bool useFrequency;
    QColor frequencyColour;

    QColor memAppsColour;
    QColor memBuffersColour;
    QColor memCachedColour;
    QColor swapUsedColour;

    QColor netReceivedColour;
    QColor netTransmittedColour;
    QColor netBothColour;
    int netMaximumSpeed;
    qreal netRealMaximumSpeed;
    bool logarithmicScale;
    int logScaleSteps;
    qreal logScaleMax;

    int historyOffset;
    QImage historyImage;

    void reset(void);

    void clearLine(void);
};


EXPORT_RAZOR_PANEL_PLUGIN_H

#endif // RAZORPANELSYSSTAT_H
