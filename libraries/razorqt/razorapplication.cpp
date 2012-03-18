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
#include <qtxdg/xdgdirs.h>
#include <QtCore/QDir>

#ifdef DEBUG
#define COLOR_DEBUG "\033[32;2m"
#define COLOR_WARN "\033[33;2m"
#define COLOR_CRITICAL "\033[31;1m"
#define COLOR_FATAL "\033[33;1m"
#define COLOR_RESET "\033[0m"

#define QAPP_NAME qApp ? qApp->objectName().toUtf8().constData() : ""

#include <cstdio>
#include <cstdlib>
#include <QDateTime>
/*! \brief Log qDebug input to file
Used only in pure Debug builds.
*/
void dbgMessageOutput(QtMsgType type, const char *msg)
 {
    FILE *f;
    QDir dir(XdgDirs::configHome().toUtf8() + "/razor");
    dir.mkpath(".");

    f = fopen (dir.absoluteFilePath("debug.log").toUtf8(), "a+");
    const char * dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    switch (type) {
    case QtDebugMsg:
        fprintf(f, "%s %s(%p) Debug: %s\n", dt, QAPP_NAME, qApp, msg);
        fprintf(stderr, "%s %s(%p) Debug: %s%s\n", COLOR_DEBUG, QAPP_NAME, qApp, msg, COLOR_RESET);
        break;
    case QtWarningMsg:
        fprintf(f, "%s %s(%p) Warning: %s\n", dt, QAPP_NAME, qApp, msg);
        fprintf(stderr, "%s %s(%p) Warning: %s%s\n", COLOR_WARN, QAPP_NAME, qApp, msg, COLOR_RESET);
        break;
    case QtCriticalMsg:
        fprintf(f, "%s %s(%p) Critical: %s\n", dt, QAPP_NAME, qApp, msg);
        fprintf(stderr, "%s %s(%p) Critical: %s%s\n", COLOR_CRITICAL, QAPP_NAME, qApp, msg, COLOR_RESET);
        break;
    case QtFatalMsg:
        fprintf(f, "%s %s(%p) Fatal: %s\n", dt, QAPP_NAME, qApp, msg);
        fprintf(stderr, "%s %s(%p) Fatal: %s%s\n", COLOR_FATAL, QAPP_NAME, qApp, msg, COLOR_RESET);
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

    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

    if (!stylesheetKey.isEmpty())
    {
        setStyleSheet(razorTheme->qss(stylesheetKey));
    }

    // TODO/FIXME: maybe move it into global config? RazorSettings::globalSettings()?
    RazorSettings s("desktop");
    bool singleClick = s.value("icon-launch-mode", "singleclick").toString() == "singleclick";
    setStyleSheet(QString("QAbstractItemView {activate-on-singleclick : %1; }").arg(singleClick ? 1 : 0));
}
