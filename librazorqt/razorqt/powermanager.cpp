#include "powermanager.h"
#include "upower.h"
#include <QDBusInterface>
#include <QMessageBox>


PowerManager::PowerManager(QObject * parent)
    : QObject(parent)
{
    m_upower = new UPower();
    connect(m_upower, SIGNAL(suspendFail()), this, SLOT(infoMessage()));
    connect(m_upower, SIGNAL(hibernateFail()), this, SLOT(infoMessage()));
    connect(m_upower, SIGNAL(monitoring(const QString &)),
            this, SLOT(monitoring(const QString&)));
}

PowerManager::~PowerManager()
{
    delete m_upower;
}

QList<QAction*> PowerManager::availableActions()
{
    QList<QAction*> ret;
    QAction * act;

    // TODO/FIXME: icons
    act = new QAction(tr("Suspend"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(suspend()));
    ret.append(act);

    act = new QAction(tr("Hibernate"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(hibernate()));
    ret.append(act);

    act = new QAction(tr("Reboot"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(reboot()));
    ret.append(act);

    act = new QAction(tr("Shutdown"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(halt()));
    ret.append(act);

    act = new QAction(tr("Logout"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(logout()));
    ret.append(act);
    
    return ret;
}
    
void PowerManager::suspend()
{
    m_upower->suspend();
}

void PowerManager::hibernate()
{
    m_upower->hibernate();
}

void PowerManager::reboot()
{
    m_upower->reboot();
}

void PowerManager::halt()
{
    m_upower->halt();
}

void PowerManager::logout()
{
    QDBusInterface interface("org.razor.session", "/RazorSession", "org.razor.session");
    interface.call( "logout" );
}

void PowerManager::infoMessage()
{
    QAction * act = qobject_cast<QAction*>(sender());
    // it hast to be called as a slot by an action
    Q_ASSERT(act);
    QMessageBox::warning(0, tr("Razor Power Manager Error"), tr("Action '%1' failed.").arg(act->text()));
}

void PowerManager::monitoring(const QString & msg)
{
    // TODO/FIXME: XDG messages?
    qDebug() << "PowerManager::monitoring" << msg;
}
