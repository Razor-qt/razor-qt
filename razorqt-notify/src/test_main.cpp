/*
 * Copyright (C) 2011 Drogolov Roman Olegovich <drull@drull.org.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 */

#include <QtGui/QApplication>
#include <QtCore/QPoint>

#include "qtnlog.h"

#include "testerapp.h"
#include "notificationserversettings.h"
#include <QtUiTools/QUiLoader>

#ifdef LOG
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#endif

int main(int argc, char *argv[])
{
    INFO("Starting test qnot2 argc=" << argc << " argv[0]=" << argv[0]);

    QApplication app(argc,argv);
    NotificationServerSettings::instance(&app);
    // from where, one has to take icons

    TesterApp tester;
    tester.show();

    return app.exec();
}
