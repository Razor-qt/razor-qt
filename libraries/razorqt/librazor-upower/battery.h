/*
 * (c)GPL3
This is probably temporary code probably. We will merge it into potential shared library.

This code is based on cool Brieuc ROBLIN's pyrotools.org applets, license GPLv3.
*/

#ifndef BATTERYMODEL_H
#define BATTERYMODEL_H

#include <QObject>

class QDBusInterface;


class Battery : public QObject {

    Q_OBJECT
    Q_ENUMS( State )
    Q_PROPERTY( QString sysfsPath READ sysfsPath STORED false )
    Q_PROPERTY( QString model READ model STORED false )
    Q_PROPERTY( QString vendor READ vendor STORED false )
    Q_PROPERTY( QString technology READ technology STORED false )
    Q_PROPERTY( QString path READ path STORED false )
    Q_PROPERTY( bool powerSupply READ powerSupply STORED false )
    Q_PROPERTY( bool hasHistory READ hasHistory STORED false )
    Q_PROPERTY( bool hasStatistics READ hasStatistics STORED false )
    Q_PROPERTY( bool isPresent READ isPresent STORED false )
    Q_PROPERTY( bool isRechargeable READ isRechargeable STORED false )
    Q_PROPERTY( double energy READ energy STORED false )
    Q_PROPERTY( double energyEmpty READ energyEmpty STORED false )
    Q_PROPERTY( double energyFull READ energyFull STORED false )
    Q_PROPERTY( double energyFullDesign READ energyFullDesign STORED false )
    Q_PROPERTY( double energyRate READ energyRate STORED false )
    Q_PROPERTY( double voltage READ voltage STORED false )
    Q_PROPERTY( double percentage READ percentage STORED false )
    Q_PROPERTY( double capacity READ capacity STORED false )
    Q_PROPERTY( Battery::State state READ state STORED false )
    Q_PROPERTY( int lowLevel READ lowLevel )
    Q_PROPERTY( double toFull READ toFull STORED false )
    Q_PROPERTY( double toEmpty READ toFull STORED false )


public:
    enum State { FullyCharged, Charging, Discharging };

    Battery( const QString & path );
    ~Battery();

//    void createView();

    bool operator ==( const Battery &other );

    //BatteryView* view() const;

    QString sysfsPath() const;
    QString model() const;
    QString vendor() const;
    QString technology() const;
    const QString & path() const;

    bool powerSupply() const;
    bool hasHistory() const;
    bool hasStatistics() const;
    bool isPresent() const;
    bool isRechargeable() const;
    bool isValid() const;

    double energy() const;
    double energyEmpty() const;
    double energyFull() const;
    double energyFullDesign() const;
    double energyRate() const;
    double voltage() const;
    double percentage() const;
    double capacity() const;
    double toFull() const;
    double toEmpty() const;
    int lowLevel() const;
    void setLowLevel(int value);


    Battery::State state() const;

private:
    QVariant getProperty( QString property ) const;
    //BatteryView         *m_view;
    QDBusInterface      *m_interfaceProps;
    QDBusInterface      *m_interface;
    QString             m_path;
    bool                m_hasAlreadyBeenLow;
    bool                m_hasAlreadyBeenFull;
    int                 m_lowLevel;

public Q_SLOTS:
    void refresh();

private Q_SLOTS:
    void changed();

Q_SIGNALS:
    void changed( Battery * );
    void lowBattery( Battery * );
    void full( Battery * );

    void monitoring( QString );

};

#endif // BATTERYMODEL_H
