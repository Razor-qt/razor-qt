#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLightDM/Greeter>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



};

#endif // MAINWINDOW_H
