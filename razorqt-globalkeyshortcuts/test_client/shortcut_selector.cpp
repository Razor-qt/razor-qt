#include "shortcut_selector.hpp"

#include <global_action_native_client.hpp>

#include <QTimer>


ShortcutSelector::ShortcutSelector(QWidget *parent)
    : QPushButton(parent)
    , mGlobalActionNativeClient(GlobalActionNativeClient::instance())
    , mShortcutTimer(new QTimer(this))
{
    setCheckable(true);

    mShortcutTimer->setInterval(1000);
    mShortcutTimer->setSingleShot(false);

    connect(this, SIGNAL(clicked()), this, SLOT(grabShortcut()));

    connect(mShortcutTimer, SIGNAL(timeout()), this, SLOT(shortcutTimer_timeout()));
    connect(mGlobalActionNativeClient, SIGNAL(grabShortcutCancelled()), this, SLOT(grabShortcut_fail()));
    connect(mGlobalActionNativeClient, SIGNAL(grabShortcutTimedout()), this, SLOT(grabShortcut_fail()));
    connect(mGlobalActionNativeClient, SIGNAL(grabShortcutFailed()), this, SLOT(grabShortcut_fail()));
    connect(mGlobalActionNativeClient, SIGNAL(shortcutGrabbed(QString)), this, SLOT(newShortcutGrabbed(QString)));
}

void ShortcutSelector::grabShortcut()
{
    if (!isChecked())
    {
        mGlobalActionNativeClient->cancelShortutGrab();
        return;
    }

    mTimeoutCounter = 10;
    mOldShortcut = text();
    setText(QString::number(mTimeoutCounter));
    mShortcutTimer->start();
    mGlobalActionNativeClient->grabShortcut(mTimeoutCounter * mShortcutTimer->interval());
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
