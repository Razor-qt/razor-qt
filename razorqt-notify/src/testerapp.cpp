#include "testerapp.h"
#include "notificationhandler.h"
#include "notification.h"

#include <QHBoxLayout>

TesterApp::TesterApp(QWidget *parent) :
    QWidget(parent),
    m_pHandler( new NotificationHandler(this)),
    m_add("add"),
    m_currNumber(0),
    m_remove("remove")
{
    QLayout* pL = new QHBoxLayout(this);
    pL->addWidget(&m_add);
    pL->addWidget(&m_remove);

    qsrand(time(NULL));

    connect ( &m_add, SIGNAL(clicked()), this, SLOT(add()));
    connect ( &m_remove, SIGNAL(clicked()), this, SLOT(remove()));
}

void TesterApp::add()
{
    QString appName("Program %1") ;
    appName = appName.arg(m_currNumber);

    Notification* pN= new Notification(appName,++m_currNumber,"notification-audio-next","","",QStringList(),QVariantMap(),15000);
    m_pHandler->addNotification(pN);
}

void TesterApp::remove()
{
    if ( m_currNumber == 0)
        return;
    int rand = qrand() % m_currNumber ;
    m_pHandler->removeNotification(rand);
    --m_currNumber;
}
