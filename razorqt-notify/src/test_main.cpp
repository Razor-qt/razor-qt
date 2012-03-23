
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

#include <QApplication>
#include <QPoint>

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
#ifdef LOG
    log4cplus::helpers::LogLog::getLogLog()->setInternalDebugging(false);
    log4cplus::SharedAppenderPtr rolling( new log4cplus::ConsoleAppender());
    rolling->setName("rollingFileAppender");
    rolling->setLayout(std::auto_ptr<log4cplus::Layout> ( new log4cplus::PatternLayout("%d{%y-%m-%d %H:%M:%S,%q} [%t] %-5p %c <%x> - %m%n")));

    log4cplus::Logger::getRoot().addAppender(rolling);
    log4cplus::Logger::getRoot().setLogLevel(log4cplus::ALL_LOG_LEVEL);
#endif //LOG

    INFO("Starting test qnot2 argc=" << argc << " argv[0]=" << argv[0]);

    QApplication app(argc,argv);
    NotificationServerSettings::instance(&app)  ;
    // from where, one has to take icons

    TesterApp tester ;
    tester.show();

    return app.exec();
}
