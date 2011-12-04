/*
This is probably temporary code probably. We will merge it into potential shared library.

This code is based on cool Brieuc ROBLIN's pyrotools.org applets, license GPLv3.
*/

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QSettings>

#include "battery.h"


Battery::Battery(const QString &path) : /*m_view(0),*/ m_path( path ) {
    m_hasAlreadyBeenLow = false;
    m_hasAlreadyBeenFull= false;

    m_interface = new QDBusInterface(
                                    "org.freedesktop.UPower",
                                    m_path,
                                    "org.freedesktop.UPower.Device",
                                    QDBusConnection::systemBus()
                                );

    m_interfaceProps = new QDBusInterface(
                                    "org.freedesktop.UPower",
                                    m_path,
                                    "org.freedesktop.DBus.Properties",
                                    QDBusConnection::systemBus()
                                );

    connect( m_interface, SIGNAL(Changed()), this, SLOT(changed()));

    refresh();
}

Battery::~Battery() {
    delete m_interface;
    delete m_interfaceProps;
//    if( m_view != 0 ) {
//        delete m_view;
//    }
}
//
//void Battery::createView() {
//    m_view = new BatteryView(this);
//}

//BatteryView* Battery::view() const {
//    return m_view;
//}

bool Battery::operator==( const Battery &other ) {
    if( ! other.isValid() ) {
        return false;
    }
    if( getProperty("NativePath") == other.getProperty("NativePath") ) {
        return true;
    }
    return false;
}

void Battery::refresh() {
    m_interface->call( "Refresh" );
}

void Battery::changed() {
    emit monitoring( tr( "%1 : Properties changed").arg( m_path ) );
    int level = percentage();
    if( level < lowLevel() ) {
        if( ! m_hasAlreadyBeenLow ) {
            m_hasAlreadyBeenLow = true;
            emit lowBattery(this);
            emit monitoring( tr( "%1 : Low" ).arg( m_path ) );
        }
    } else {
        m_hasAlreadyBeenLow = false;
    }

    if( state() == FullyCharged ) {
        if( ! m_hasAlreadyBeenFull ) {
            m_hasAlreadyBeenFull = true;
            emit full(this);
            emit monitoring( tr( "%1 : Full" ).arg( m_path ) );
        }
    } else {
        m_hasAlreadyBeenFull = false;
    }
    //m_view->refresh();
    emit changed(this);
}

bool Battery::isValid() const {
    return getProperty("Type").toInt() == 2;
}

QVariant Battery::getProperty( QString property ) const {
    QList<QVariant> arg = m_interfaceProps->call( "Get", m_interface->interface(), property ).arguments();
    if( arg.count() >= 1 ) {
        return arg.at(0).value<QDBusVariant>().variant();
    } else {
        return QVariant(false);
    }
}

int Battery::lowLevel() const {
    QSettings settings;
    settings.beginGroup("Batteries_Low_Level");
        int lvl = settings.value(m_path, 20).toInt();
    settings.endGroup();
    return lvl;
}

void Battery::setLowLevel(int value) {
    QSettings settings;
    settings.beginGroup("Batteries_Low_Level");
        settings.setValue(m_path, value);
    settings.endGroup();
}

QString Battery::sysfsPath() const {
    return getProperty( "NativePath" ).toString();
}

QString Battery::model() const {
    return getProperty( "Model" ).toString();
}

QString Battery::vendor() const {
    return getProperty( "Vendor" ).toString();
}

QString Battery::technology() const {
    return getProperty( "Technology").toString();
}

const QString & Battery::path() const {
    return m_path;
}

bool Battery::powerSupply() const {
    return getProperty( "PowerSupply" ).toBool();
}

bool Battery::hasHistory() const {
    return getProperty( "HasHistory" ).toBool();
}

bool Battery::hasStatistics() const {
    return getProperty( "HasStatistics" ).toBool();
}

bool Battery::isPresent() const {
    return getProperty( "IsPresent" ).toBool();
}

bool Battery::isRechargeable() const {
    return getProperty( "IsRechargeable" ).toBool();
}

double Battery::energy() const {
    return getProperty( "Energy" ).toDouble();
}

double Battery::energyEmpty() const {
    return getProperty( "EnergyEmpty" ).toDouble();
}

double Battery::energyFull() const {
    return getProperty( "EnergyFull" ).toDouble();
}

double Battery::energyFullDesign() const {
    return getProperty( "EnergyFullDesign" ).toDouble();
}

double Battery::energyRate() const {
    return getProperty( "EnergyRate" ).toDouble();
}

double Battery::voltage() const {
    return getProperty( "Voltage" ).toDouble();
}

double Battery::percentage() const {
    return getProperty( "Percentage" ).toDouble();
}

double Battery::capacity() const {
    return getProperty( "Capacity" ).toDouble();
}

double Battery::toEmpty() const {
    return getProperty( "TimeToEmpty" ).toDouble();
}

double Battery::toFull() const {
    return getProperty( "TimeToFull" ).toDouble();
}

Battery::State Battery::state() const {
    uint state = getProperty( "State" ).toUInt();
    switch( state ) {
        case (uint) 0:
            return FullyCharged;
            break;

        case (uint) 1:
            return Charging;
            break;

        case (uint) 2:
            return Discharging;
            break;

        default:
            return FullyCharged;
            break;
    }
}
