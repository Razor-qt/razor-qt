#include "shortcut_selector.h"

#include <razor-global-key-shortcuts-client/razor-global-key-shortcuts-client.h>

#include <QTimer>
#include <QAction>
#include <QMenu>


ShortcutSelector::ShortcutSelector(QWidget *parent)
    : QToolButton(parent)
    , mClient(GlobalKeyShortcut::Client::instance())
    , mShortcutTimer(new QTimer(this))
{
    setCheckable(true);
    setFocusPolicy(Qt::StrongFocus);

    mShortcutTimer->setInterval(1000);
    mShortcutTimer->setSingleShot(false);

    connect(this, SIGNAL(clicked()), this, SLOT(grabShortcut()));

    connect(mShortcutTimer, SIGNAL(timeout()), this, SLOT(shortcutTimer_timeout()));
    connect(mClient, SIGNAL(grabShortcutCancelled()), this, SLOT(grabShortcut_fail()));
    connect(mClient, SIGNAL(grabShortcutTimedout()), this, SLOT(grabShortcut_fail()));
    connect(mClient, SIGNAL(grabShortcutFailed()), this, SLOT(grabShortcut_fail()));
    connect(mClient, SIGNAL(shortcutGrabbed(QString)), this, SLOT(newShortcutGrabbed(QString)));
}

void ShortcutSelector::grabShortcut()
{
    if (!isChecked())
    {
        mClient->cancelShortutGrab();
        return;
    }

    mTimeoutCounter = 10;
    mOldShortcut = text();
    setText(QString::number(mTimeoutCounter));
    mShortcutTimer->start();
    mClient->grabShortcut(mTimeoutCounter * mShortcutTimer->interval());
}

void ShortcutSelector::shortcutTimer_timeout()
{
    --mTimeoutCounter;
    setText(QString::number(mTimeoutCounter));
    if (!mTimeoutCounter)
    {
        setChecked(false);
        mShortcutTimer->stop();
    }
}

void ShortcutSelector::grabShortcut_fail()
{
    setChecked(false);
    mShortcutTimer->stop();
    setText(mOldShortcut);
    emit shortcutGrabbed(mOldShortcut);
}

void ShortcutSelector::newShortcutGrabbed(const QString &newShortcut)
{
    setChecked(false);
    mShortcutTimer->stop();
//    setText(newShortcut);
    setText(QString());
    emit shortcutGrabbed(newShortcut);
}

void ShortcutSelector::clear()
{
    setText(QString());
}

QAction * ShortcutSelector::addMenuAction(const QString &title)
{
    QMenu *subMenu = menu();
    if (!subMenu)
    {
        setPopupMode(QToolButton::MenuButtonPopup);
        subMenu = new QMenu(this);
        setMenu(subMenu);
    }
    QAction *action = new QAction(title, subMenu);
    subMenu->addAction(action);
    return action;
}
