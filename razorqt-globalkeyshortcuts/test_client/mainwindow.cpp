#include "mainwindow.hpp"
#include <global_action_native_client.hpp>

#include <QDBusConnection>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mGlobalActionNativeClient(GlobalActionNativeClient::instance())
    , mGlobalActionCheckbox(0)
{
    setupUi(this);

    QDBusConnection::sessionBus().registerService("org.test.app");

    mGlobalActionCheckbox = mGlobalActionNativeClient->addAction("Alt+T", "checkbox", "Test checkbox toggle");

    if (mGlobalActionCheckbox)
    {
        connect(mGlobalActionCheckbox, SIGNAL(activated()), this, SLOT(checkboxShortcutActivated()));
        connect(mGlobalActionCheckbox, SIGNAL(shortcutChanged(QString,QString)), this, SLOT(checkboxShortcutChanged(QString,QString)));

        changeShortcut->setText(mGlobalActionCheckbox->shortcut());
    }
    else
        changeShortcut->setEnabled(false);

    connect(changeShortcut, SIGNAL(shortcutGrabbed(QString)), this, SLOT(shortcutGrabbed(QString)));

    connect(mGlobalActionNativeClient, SIGNAL(daemonPresenceChanged(bool)), changeShortcut, SLOT(setEnabled(bool)));
    changeShortcut->setEnabled(mGlobalActionNativeClient->isDaemonPresent());
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::shortcutGrabbed(const QString &newShortcut)
{
    mGlobalActionCheckbox->changeShortcut(newShortcut);
}

void MainWindow::checkboxShortcutChanged(const QString &/*oldShortcut*/, const QString &newShortcut)
{
    changeShortcut->setText(newShortcut);
}

void MainWindow::checkboxShortcutActivated()
{
    showAction->toggle();
}
