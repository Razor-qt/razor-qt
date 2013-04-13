#ifndef SHORTCUT_SELECTOR_HPP
#define SHORTCUT_SELECTOR_HPP

#include <QToolButton>
#include <QWidget>
#include <QString>


class QTimer;

namespace GlobalKeyShortcut
{
class Client;
}

class ShortcutSelector : public QToolButton
{
    Q_OBJECT
public:
    explicit ShortcutSelector(QWidget *parent = 0);

    QAction *addMenuAction(const QString &title);

    bool shortcutAutoApplied(void) const { return mAutoApplyShortcut; }

signals:
    void shortcutGrabbed(const QString &);
    void cleared();

public slots:
    void grabShortcut();

    void clear();

    void autoApplyShortcut(bool value = true) { mAutoApplyShortcut = value; }

private slots:
    void shortcutTimer_timeout();
    void grabShortcut_fail();
    void newShortcutGrabbed(const QString &);

private:
    GlobalKeyShortcut::Client *mClient;
    QString mOldShortcut;
    int mTimeoutCounter;
    QTimer *mShortcutTimer;
    bool mAutoApplyShortcut;
};

#endif // SHORTCUT_SELECTOR_HPP
