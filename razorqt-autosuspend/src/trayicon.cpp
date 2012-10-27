/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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

#include "trayicon.h"
#include "razorqt/razorsettings.h"
#include "../config/constants.h"
#include <math.h>
#include <QtCore/QDebug>
#include <QtSvg/QSvgRenderer>
#include <QtGui/QPainter>

TrayIcon::TrayIcon(Battery* battery, QObject *parent) : 
    QSystemTrayIcon(parent), 
        mBattery(battery), 
        mSettings("razor-autosuspend")
{
    connect(mBattery, SIGNAL(batteryChanged()), this, SLOT(update()));
    connect(RazorSettings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(iconThemeChanged()));
    connect(&mSettings, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(showStatus(QSystemTrayIcon::ActivationReason)));

    checkThemeStatusIcons(); 
    update(); 
    setVisible(mSettings.value(SHOWTRAYICON_KEY, true).toBool());
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::update()
{
    updateStatusIcon();
    updateToolTip();
    mBatteryInfo.updateInfo(mBattery);
}


void TrayIcon::updateStatusIcon()
{
    static double chargeLevel = -1;
    static bool   discharging = false;

    if (fabs(mBattery->chargeLevel() - chargeLevel) < 0.1 && (mBattery->discharging() == discharging))
    {
        qDebug() << "No significant change - not updating icon";
        return;
    }

    chargeLevel = mBattery->chargeLevel();
    discharging = mBattery->discharging();

    if (mSettings.value(USETHEMEICONS_KEY, true).toBool() && mThemeHasStatusIcons)
    {
        QString iconName;
        
        if (QIcon::themeName() == "oxygen")
        {
            if (chargeLevel < 20)        iconName = discharging ? "battery-low" : "battery-charging-low";
            else if (chargeLevel < 40)   iconName = discharging ? "battery-caution" : "battery-charging-caution";
            else if (chargeLevel < 60)   iconName = discharging ? "battery-040" : "battery-charging-040";
            else if (chargeLevel < 80)   iconName = discharging ? "battery-060" : "battery-charging-060";
            else if (chargeLevel < 99.5) iconName = discharging ? "battery-080" : "battery-charging-080";
            else                         iconName = discharging ? "battery-100" : "battery-charging";
        }
        else // For all themes but 'oxygen' we follow freedesktop's battery status icon name standard 
             // (http://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html) _with_ the changes proposed in
             // https://bugs.freedesktop.org/show_bug.cgi?id=41458 (we assume that this patch will be accepted)
        {
            if      (chargeLevel <  1 && discharging) iconName = "battery-empty" ;
            else if (chargeLevel < 20)                iconName = discharging ? "battery-caution" : "battery-caution-discharging";
            else if (chargeLevel < 40)                iconName = discharging ? "battery-low" : "battery-low-discharging";
            else if (chargeLevel < 60)                iconName = discharging ? "battery-good" : "battery-good-discharging";
            else                                      iconName = discharging ? "battery-full" : "battery-full-discharging";
        }
        
        qDebug() << "ChargeLevel" << chargeLevel 
                 << "- getting icon:"  << iconName 
                 << "from" << QIcon::themeName() << "theme";
        setIcon(QIcon::fromTheme(iconName));
    }
    else
    {
        setIcon(getBuiltInIcon(chargeLevel, discharging));
    }
}

void TrayIcon::updateToolTip()
{
    QString toolTip = mBattery->stateAsString();

    if (mBattery->state() == 1 || mBattery->state() == 2)
    {
        toolTip = toolTip + QString(" - %1 %").arg(mBattery->chargeLevel(), 0, 'f', 1);
    }
    setToolTip(toolTip);
}



void TrayIcon::checkThemeStatusIcons()
{
    mThemeHasStatusIcons = true; 
    if ("oxygen" != QIcon::themeName())
    {
        // We know what icons the oxygen theme contains, so with oxygen we're good
        // but we don't know that all icon themes have those that the freedesktop
        // standard prescribes. If the current theme doesn't, we will fall back to 
        // the built in status icons.

        static QStringList statusIconNames = QStringList() 
            <<  "battery-empty" 
            << "battery-caution"           << "battery-low"          << "battery-good"          << "battery-full"
            <<  "battery-caution-charging" << "battery-low-charging" << "battery-good-charging" << "battery-full-charging";

        foreach (QString statusIconName, statusIconNames) 
        {
            if (! QIcon::hasThemeIcon(statusIconName))
            {
                mThemeHasStatusIcons = false;
                break;
            }
        }
    }
}

void TrayIcon::iconThemeChanged()
{
    checkThemeStatusIcons();
    updateStatusIcon();
}

void TrayIcon::settingsChanged()
{
    updateStatusIcon();
    setVisible(mSettings.value(SHOWTRAYICON_KEY, true).toBool()); 
}


QIcon TrayIcon::getBuiltInIcon(double chargeLevel, bool discharging)
{
        // See http://www.w3.org/TR/SVG/Overview.html 
    // and regarding circle-arch in particular: 
    // http://www.w3.org/TR/SVG/paths.html#PathDataEllipticalArcCommands
   
    // We show charge with a segment of a circle.
    // We start at the top of the circle
    // The starting point of the circle segment is at the top (12 o'clock or (0,1) or pi/2 
    // and it moves counter-clockwise as the charge increases
    // First we calculate in floating point numbers, using a circle with center
    // in (0,0) and a radius of 1
    double angle = 2*M_PI*chargeLevel/100 + M_PI_2;
    qDebug() << "Angle:"  << angle;
    double segment_endpoint_x = cos(angle);
    double segment_endpoint_y = sin(angle);

    // svg uses an coordinate system with (0,0) at the topmost left corner,  
    // y increasing downwards and x increasing left-to-right.
    // We draw the circle segments with center at (100,100) and radius 100 for the
    // outer and radius 60 for the inner. The segments will (unless fully charged, 
    // where they go full circle) be radially connected at the endpoints.
    QString chargeGraphics;

    if (chargeLevel < 0.5)
    {
        chargeGraphics = "";
    }
    else if (chargeLevel > 99.5)
    {
        chargeGraphics =
            "<path d='M 100,0 A 100,100 0 1,0 101,0 z M 100,40 A 60,60 0 1,0 101,40 z' stroke-width='0'/>";
    }
    else {
        chargeGraphics = 
            QString("<path d='M 100,0 A 100,100 0 %1,0 %2,%3 L %4,%5 A 60,60 0 %1,1 100,40 z'/>")
                .arg(angle > M_PI + M_PI_2 ? 1 : 0)     // %1 
                .arg(round(100*(1 + segment_endpoint_x)))    // %2
                .arg(round(100*(1 - segment_endpoint_y)))    // %3
                .arg(round(100*(1 + 0.6*segment_endpoint_x)))    // %4
                .arg(round(100*(1 - 0.6*segment_endpoint_y)));   // %5
    }
 
    QString chargeColor;
    int lowLevel = mSettings.value(POWERLOWLEVEL_KEY, 5).toInt();
    if (discharging && chargeLevel <= lowLevel + 10)
    {
        chargeColor = "rgb(200,40,40)";
    }
    else if (discharging && chargeLevel <= lowLevel + 30) 
    {
        int fac = chargeLevel - lowLevel - 10;
        chargeColor = QString("rgb(%1,%2,40)").arg(40 + 200 - fac*8).arg(40 + fac*8);
    }
    else 
        chargeColor = "rgb(40,200,40)";


    QString sign = 
            discharging ? 
                QString("<path d='M 60,100 h 80' stroke='black' stroke-width='20' />"):                // Minus
                QString("<path d='M 60,100 h 80 M 100,60 v 80' stroke='black' stroke-width='20' />");  // Plus

    QString svg = 
        QString("<?xml version='1.0' standalone='no'?>\n"
                "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
                "<svg xmlns='http://www.w3.org/2000/svg'  version='1.1' viewBox='0 0 210 210'>\n"
                "    <circle cx='100' cy='100' r='100' fill='rgb(210,210,210)' stroke-width='0'/>\n"
                "    <circle cx='100' cy='100' r='55' fill='rgb(255,255,255)' stroke-width='0'/>\n"
                "    <g fill-rule='evenodd' fill='%2' stroke-width='0'>\n"
                "        %1\n"
                "    </g>\n"
                "    %3\n" 
                "</svg>\n").arg(chargeGraphics).arg(chargeColor).arg(sign);

    qDebug() << svg;

    // Paint the svg on a pixmap and create an icon from that.
    QSvgRenderer render(svg.toAscii());
    QPixmap pixmap(render.defaultSize());
    pixmap.fill(QColor(0,0,0,0));
    QPainter painter(&pixmap);
    render.render(&painter);
    return QIcon(pixmap);

}

/*QIcon TrayIcon::getBuiltInIcon(double chargeLevel, bool discharging)
{
      // See http://www.w3.org/TR/SVG/Overview.html 
    // and regarding circle-arch in particular: 
    // http://www.w3.org/TR/SVG/paths.html#PathDataEllipticalArcCommands
    static const QString svgTemplate =
        "<?xml version='1.0' standalone='no'?>\n"
        "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN' 'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>\n"
        "<svg xmlns='http://www.w3.org/2000/svg'  version='1.1' viewBox='0 0 200 200'>\n"
        "   %1\n"
        "   <circle cx='100' cy='100' r='55' fill='white'/>\n"
        "   %2\n"
        "</svg>\n";
    
    // We show charge with a segment of a circle.
    // We start at the top of the circle
    // The starting point of the circle segment is at the top (12 o'clock or (0,1) or pi/2 
    // and it moves counter-clockwise as the charge increases
    // First we calculate in floating point numbers, using a circle with center
    // in (0,0) and a radius of 1
    double angle = 2*M_PI*chargeLevel/100 + M_PI_2; 
    double segment_endpoint_x = cos(angle);
    double segment_endpoint_y = sin(angle);
    int large_arch = (angle > M_PI + M_PI_2 ? 1 : 0); // 1 if the arch is more than half a circle, 0 otherwise

    // svg uses an coordinate system with (0,0) at the topmost left corner,  
    // y increasing downwards and x increasing left-to-right.
    // We draw the circle with center at (100,100) and radius 100 so that it 
    // fits inside a viewBox of (0 0 200 200).
    int x_start = 100;
    int y_start =   0;
    int x_end = round(100 + 100*segment_endpoint_x);
    int y_end = round(100 - 100*segment_endpoint_y);
    int x_rel = x_end - x_start;
    int y_rel = y_end - y_start;

    // We color the arch red when charge level < 30% and discharging, green otherwise
    static const QString warningcolor = "rgb(200, 40, 40)"; 
    static const QString green = "rgb(40, 200, 40)";
    bool warn_level = chargeLevel < 30 && discharging;
    QString chargeArch = 
        QString("<path d='M100,100 v -100 a100,100 0 %1,0 %2,%3 z' fill='%4' stroke-width='0'/>")
            .arg(large_arch)
            .arg(x_rel)
            .arg(y_rel)
            .arg(warn_level ? warningcolor : green);
   
    //qDebug() << "chargeArch:" << chargeArch;
    // We color the minus sign red when chargelevel below 30%, black otherwise
    QString minus =
        QString("<path d='M 60,100 h 80' stroke='%1' stroke-width='20'/>")
            .arg(warn_level ? warningcolor : "black");
 
    static QString plus =
        "<path d='M 60,100 h 80 M 100,60 v 80' stroke='black' stroke-width='20'/>";
 
    QString svg = svgTemplate.arg(chargeArch).arg(discharging ? minus : plus);

   qDebug() << svg;

    // Paint the svg on a pixmap and create an icon from that.
    QSvgRenderer render(svg.toAscii());
    QPixmap pixmap(render.defaultSize());
    pixmap.fill(QColor(0,0,0,0));
    QPainter painter(&pixmap);
    render.render(&painter);
    return QIcon(pixmap);
}*/

void TrayIcon::showStatus(ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        if (mBatteryInfo.isVisible())
        {
            mBatteryInfo.close();
        }
        else
        {
            mBatteryInfo.open();
        }
    }
}

