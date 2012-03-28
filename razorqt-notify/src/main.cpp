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

#include <signal.h>
#include <sys/stat.h>
#include <pwd.h>
#include <QtServiceBase>
#include <QApplication>

#include "qtnlog.h"
#include "qtndbusconnector.h"

#ifdef LOG
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#endif

class QNotificationService : public QtService<QApplication>
{
public:
    QNotificationService(int argc, char* argv[]);
public:
    void start();
private:
    QtnDbusConnector* m_pConnector ;
};

QNotificationService::QNotificationService(int argc, char *argv[]):
    QtService<QApplication>(argc, argv, "qtnotification daemon"),
    m_pConnector(NULL)
{
    setServiceDescription("A dummy HTTP service implemented with Qt");
    setStartupType(QtServiceController::AutoStartup);
    setServiceFlags(QtServiceBase::CannotBeStopped);
}

void QNotificationService::start()
{
    QApplication* pCoreApp = application();
    INFO("Daemon started!");

    try
    {
        m_pConnector = new QtnDbusConnector(pCoreApp);
        TRACE("m_pConnector=" << m_pConnector);

        m_pConnector->connectToDbus();
    }
    catch ( const std::exception& ex)
    {
        WARN("Exception caught ex.what()" << ex.what());
        stop();
    }
}

int main(int argc, char *argv[])
{
    INFO("Starting qtnot2 argc=" << argc << " argv[0]=" << argv[0]);
    Q_INIT_RESOURCE(images);
    if ( argc == 2)
    {
        INFO(argv[1]);
        std::string arg(argv[1]);
        if ( arg.compare("--no-daemon")==0)
        {

            QApplication app( argc,argv );
            Q_INIT_RESOURCE(images);
            INFO("Daemon started!");
            int iRet = 0 ;
            try
            {
                QtnDbusConnector* pDbus = new QtnDbusConnector(&app);

                pDbus->connectToDbus();
                iRet= app.exec();
            }
            catch( const std::exception& ex)
            {
                FATAL(" Caught exception. ex.what()=" << ex.what());
            }

            return iRet ;

        }
    }
    else
    {
        QNotificationService srv(argc,argv) ;
        return srv.exec();
    }
}
