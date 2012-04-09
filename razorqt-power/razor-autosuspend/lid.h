#ifndef LID_H
#define LID_H

#include <QDBusInterface>

class Lid : public QObject
{
    Q_OBJECT

signals:
    void changed(bool closed);

public:
    Lid();
    bool isClosed() { return closed; }

private:
    bool closed;
    QDBusInterface *uPower;
    QDBusInterface *uPowerProperties;

private slots:
    void uPowerChange();
};

#endif // LID_H
