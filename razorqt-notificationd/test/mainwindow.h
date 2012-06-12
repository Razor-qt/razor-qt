#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class RazorNotification;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    void createNotification();
    void clickEvent(uint button);

private:
    Ui::MainWindow* ui;
    QList<RazorNotification*> nlist;
};

#endif // MAINWINDOW_H
