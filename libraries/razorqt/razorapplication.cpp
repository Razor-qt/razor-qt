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

#include <QtCore/QDir>

#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgdirs.h>

#include "razorapplication.h"
#include "razorsettings.h"


#define COLOR_DEBUG "\033[32;2m"
#define COLOR_WARN "\033[33;2m"
#define COLOR_CRITICAL "\033[31;1m"
#define COLOR_FATAL "\033[33;1m"
#define COLOR_RESET "\033[0m"

#define QAPP_NAME qApp ? qApp->objectName().toUtf8().constData() : ""

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <QDateTime>
/*! \brief Log qDebug input to file
Used only in pure Debug builds or when is the system environment
variable RAZOR_DEBUG set
*/
void dbgMessageOutput(QtMsgType type, const char *msg)
{
    QDir dir(XdgDirs::configHome().toUtf8() + "/razor");
    dir.mkpath(".");

    const char* typestr;
    const char* color;
    switch (type) {
    case QtDebugMsg:
        typestr = "Debug";
        color = COLOR_DEBUG;
        break;
    case QtWarningMsg:
        typestr = "Warning";
        color = COLOR_WARN;
        break;
    case QtFatalMsg:
        typestr = "Fatal";
        color = COLOR_FATAL;
        break;
    default: // QtCriticalMsg
        typestr = "Critical";
        color = COLOR_CRITICAL;
    }

    QByteArray dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    if (isatty(STDERR_FILENO))
        fprintf(stderr, "%s %s(%p) %s: %s%s\n", color, QAPP_NAME, qApp, typestr, msg, COLOR_RESET);
    else
        fprintf(stderr, "%s(%p) %s: %s\n", QAPP_NAME, qApp, typestr, msg);

    FILE *f = fopen(dir.absoluteFilePath("debug.log").toUtf8().constData(), "a+");
    fprintf(f, "%s %s(%p) %s: %s\n", dt.constData(), QAPP_NAME, qApp, typestr, msg);
    fclose(f);

    if (type == QtFatalMsg)
        abort();
}

RazorApplication::RazorApplication(int &argc, char** argv)
    : QApplication(argc, argv)
{
#ifdef DEBUG
    qInstallMsgHandler(dbgMessageOutput);
#else
    if (!qgetenv("RAZOR_DEBUG").isNull())
        qInstallMsgHandler(dbgMessageOutput);
#endif

    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));
    connect(RazorSettings::globalSettings(), SIGNAL(razorThemeChanged()), this, SLOT(updateTheme()));
    updateTheme();
}


void RazorApplication::updateTheme()
{
    QString styleSheetKey = QFileInfo(applicationFilePath()).fileName();
    setStyleSheet(razorTheme.qss(styleSheetKey));
    emit themeChanged();
}
