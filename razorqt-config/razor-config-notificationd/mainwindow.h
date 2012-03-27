#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "razorsettings.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void showNotification();
    void positionChanged();
    void sizeChanged();
    void opacityChanged(int value);
    
private:
    Ui::MainWindow *ui;
    RazorSettings m_settings ;
    quint32 m_notificationId ;
};

#endif // MAINWINDOW_H
