#ifndef TESTERAPP_H
#define TESTERAPP_H

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtCore/QPointer>

class NotificationHandler;

class TesterApp : public QWidget
{
    Q_OBJECT

public:
    explicit TesterApp(QWidget *parent = 0);
    
signals:
    
public slots:
private slots:
    void add();
    void remove();
private:
    NotificationHandler *m_pHandler;
    QPushButton m_add;
    int m_currNumber;
    QPushButton m_remove;  
};

#endif // TESTERAPP_H
