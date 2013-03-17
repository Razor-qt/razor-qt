#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "ui_mainwindow.h"

class GlobalActionNativeClient;
class GlobalAction;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);

private slots:
    void checkboxShortcutChanged(const QString &oldShortcut, const QString &newShortcut);
    void checkboxShortcutActivated();
    void shortcutGrabbed(const QString &newShortcut);

protected:
    void changeEvent(QEvent *e);

private:
    GlobalActionNativeClient *mGlobalActionNativeClient;
    GlobalAction *mGlobalActionCheckbox;
};

#endif // MAINWINDOW_HPP
