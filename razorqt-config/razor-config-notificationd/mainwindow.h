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

    void positionXChanged();
    void positionYChanged();

    void sizeXChanged();
    void sizeYChanged();
    
private:
    Ui::MainWindow *ui;
    RazorSettings m_settings ;
};

#endif // MAINWINDOW_H
