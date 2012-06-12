#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <razorqt/razornotification.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()), SLOT(createNotification()));
    RazorNotification::notify("1");
    RazorNotification::notify("2");
    RazorNotification::notify("3");
}

void MainWindow::createNotification()
{
    RazorNotification* n = new RazorNotification("Notification!");
    nlist.append(n);
    connect(n, SIGNAL(actionActivated(uint)), this, SLOT(clickEvent(uint)));
    n->setActions(QStringList() << "Hi" << "Bye" << "foo" << "bar" << "lorem" << "ipsum", 1);
    n->setIcon("preferences-desktop-launch-feedback");
    n->update();
}

void MainWindow::clickEvent(uint button)
{
    RazorNotification* n = qobject_cast<RazorNotification*>(sender());
    if (n)
    {
        if (button == 0)
        {
            n->setBody("Hello!");
        }
        else
        {
            n->setBody("Closing...");
            n->setTimeout(1000);
        }
        n->update();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    qDeleteAll(nlist);
}
