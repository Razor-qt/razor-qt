#ifndef SHORTCUT_SELECTOR_HPP
#define SHORTCUT_SELECTOR_HPP

#include <QPushButton>
#include <QWidget>
#include <QString>


class GlobalActionNativeClient;
class QTimer;

class ShortcutSelector : public QPushButton
{
    Q_OBJECT
public:
    explicit ShortcutSelector(QWidget *parent = 0);
    
signals:
    void shortcutGrabbed(const QString &);

public slots:
    void grabShortcut();

private slots:
    void shortcutTimer_timeout();
    void grabShortcut_fail();
    void newShortcutGrabbed(const QString &);

private:
    GlobalActionNativeClient *mGlobalActionNativeClient;
    QString mOldShortcut;
    int mTimeoutCounter;
    QTimer *mShortcutTimer;
};

#endif // SHORTCUT_SELECTOR_HPP
