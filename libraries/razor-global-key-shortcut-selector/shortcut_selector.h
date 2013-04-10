#ifndef SHORTCUT_SELECTOR_HPP
#define SHORTCUT_SELECTOR_HPP

#include <QPushButton>
#include <QWidget>
#include <QString>


class QTimer;

namespace GlobalKeyShortcut
{
class Client;
}

class ShortcutSelector : public QPushButton
{
    Q_OBJECT
public:
    explicit ShortcutSelector(QWidget *parent = 0);

signals:
    void shortcutGrabbed(const QString &);

public slots:
    void grabShortcut();

    void clear();

private slots:
    void shortcutTimer_timeout();
    void grabShortcut_fail();
    void newShortcutGrabbed(const QString &);

private:
    GlobalKeyShortcut::Client *mClient;
    QString mOldShortcut;
    int mTimeoutCounter;
    QTimer *mShortcutTimer;
};

#endif // SHORTCUT_SELECTOR_HPP
