#include "powermanager.h"
#include "upower.h"
#include "xdgicon.h"
#include <QDBusInterface>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopWidget>

class MessageBox: public QMessageBox
{
public:
    explicit MessageBox(QWidget *parent = 0): QMessageBox(parent) {}

    static QMessageBox::StandardButton question(QWidget* parent,
                                         const QString& title,
                                         const QString& text,
                                         StandardButton button0,
                                         StandardButton button1)
    {
        MessageBox msgBox(parent);
        msgBox.setWindowTitle(title);
        msgBox.setText(text);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if (msgBox.exec() == QMessageBox::Yes)
            return QMessageBox::Yes;
        else
            return QMessageBox::No;
    }


protected:
    virtual void resizeEvent(QResizeEvent* event)
    {
        QRect screen = QApplication::desktop()->screenGeometry();
        move((screen.width()  - this->width()) / 2,
             (screen.height() - this->height()) / 2);

    }
};

PowerManager::PowerManager(QObject * parent)
    : QObject(parent),
    m_parentWidget(0)
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
    act = new QAction(XdgIcon::fromTheme("system-suspend-hibernate"), tr("Hibernate"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(hibernate()));
    ret.append(act);
    
    act = new QAction(XdgIcon::fromTheme("system-suspend"), tr("Suspend"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(suspend()));
    ret.append(act);

    act = new QAction(XdgIcon::fromTheme("system-reboot"), tr("Reboot"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(reboot()));
    ret.append(act);

    act = new QAction(XdgIcon::fromTheme("system-shutdown"), tr("Shutdown"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(halt()));
    ret.append(act);

    act = new QAction(XdgIcon::fromTheme("system-log-out"), tr("Logout"), this);
    connect(act, SIGNAL(triggered()), this, SLOT(logout()));
    ret.append(act);

    return ret;
}
    



void PowerManager::suspend()
{
     if (MessageBox::question(m_parentWidget, tr("Razor Session Suspend"),
                            tr("Do you want to really suspend your computer?<p>Suspends the computer into a low power state. System state is not preserved if the power is lost."),
                            QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_upower->suspend();
}

void PowerManager::hibernate()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Hibernate"),
                             tr("Do you want to really hibernate your computer?<p>Hibernates the computer into a low power state. System state is preserved if the power is lost."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_upower->hibernate();
}

void PowerManager::reboot()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Reboot"),
                             tr("Do you want to really restart your computer? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_upower->reboot();
}

void PowerManager::halt()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Shutdown"),
                             tr("Do you want to really switch off your computer? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    m_upower->halt();
}

void PowerManager::logout()
{
    if (MessageBox::question(m_parentWidget, tr("Razor Session Logout"),
                             tr("Do you want to really logout? All unsaved work will be lost..."),
                             QMessageBox::Yes, QMessageBox::No) == QMessageBox::No)
    {
        return;
    }

    QDBusInterface interface("org.razor.session", "/RazorSession", "org.razor.session");
    interface.call( "logout" );
}

void PowerManager::infoMessage()
{
    QAction * act = qobject_cast<QAction*>(sender());
    // it hast to be called as a slot by an action
    Q_ASSERT(act);
    QMessageBox::warning(m_parentWidget, tr("Razor Power Manager Error"), tr("Action '%1' failed.").arg(act->text()));
}

void PowerManager::monitoring(const QString & msg)
{
    // TODO/FIXME: XDG messages?
    qDebug() << "PowerManager::monitoring" << msg;
}
