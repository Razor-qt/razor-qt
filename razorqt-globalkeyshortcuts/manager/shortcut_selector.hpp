#ifndef SHORTCUT_SELECTOR_HPP
#define SHORTCUT_SELECTOR_HPP

#include <QPushButton>
#include <QWidget>
#include <QString>


class Actions;
class QTimer;

class ShortcutSelector : public QPushButton
{
    Q_OBJECT
public:
    explicit ShortcutSelector(Actions *actions, QWidget *parent = 0);
    explicit ShortcutSelector(QWidget *parent = 0);
    void setActions(Actions *actions);

signals:
    void shortcutGrabbed(const QString &);

public slots:
    void grabShortcut(int timeout = 10);

private slots:
    void shortcutTimer_timeout();
    void grabShortcut_fail();
    void newShortcutGrabbed(const QString &);

private:
    Actions *mActions;
    QString mOldShortcut;
    int mTimeoutCounter;
    QTimer *mShortcutTimer;

    void init();
};

#endif // SHORTCUT_SELECTOR_HPP
