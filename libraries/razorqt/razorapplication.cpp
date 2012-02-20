/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012-2013 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "razorapplication.h"
#include "razorsettings.h"
#include <qtxdg/xdgicon.h>

#ifdef DEBUG
#include <cstdio>
#include <cstdlib>
#include <QDateTime>
/*! \biref Log qDebug input to file
Used only in pure Debug builds.
*/
void dbgMessageOutput(QtMsgType type, const char *msg)
 {
    FILE *f;
    f = fopen (razorConfigDir().toUtf8() + "/debug.log", "a+");
    const char * dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    switch (type) {
    case QtDebugMsg:
        fprintf(f, "%s Debug: %s\n", dt, msg);
        break;
    case QtWarningMsg:
        fprintf(f, "%s Warning: %s\n", dt, msg);
        printf("%s Warning: %s\n", dt, msg);
        break;
    case QtCriticalMsg:
        fprintf(f, "%s Critical: %s\n", dt, msg);
        printf("%s Critical: %s\n", dt, msg);
        break;
    case QtFatalMsg:
        fprintf(f, "%s Fatal: %s\n", dt, msg);
        printf("%s Fatal: %s\n", dt, msg);
        fclose(f);
        abort();
    }
    fclose(f);
}
#endif

RazorApplication::RazorApplication(int &argc, char** argv, const QString &stylesheetKey)
    : QApplication(argc, argv)
{
#ifdef DEBUG
    qInstallMsgHandler(dbgMessageOutput);
#endif

    setStyle(new RazorQProxyStyle());
    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

    if (!stylesheetKey.isEmpty())
    {
        setStyleSheet(razorTheme->qss(stylesheetKey));
    }
}



RazorQProxyStyle::RazorQProxyStyle(QStyle * style)
    : QProxyStyle(style)
{
    // TODO/FIXME: maybe move it into global config? RazorSettings::globalSettings()?
    RazorSettings s("desktop");
    m_singleClick = s.value("icon-launch-mode", "singleclick").toString() == "singleclick";
}

int RazorQProxyStyle::styleHint(StyleHint hint,
                                const QStyleOption *option,
                                const QWidget *widget,
                                QStyleHintReturn *returnData) const
{
    if (hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
        return m_singleClick ? 1 : 0;

    return QProxyStyle::styleHint(hint, option, widget, returnData);
}
