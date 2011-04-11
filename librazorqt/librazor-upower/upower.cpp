 #include "upower.h"

UPower::UPower() : m_interfaceProps(0), m_interface(0) {
    m_watcher = new QDBusServiceWatcher;
    m_watcher->addWatchedService("org.freedesktop.UPower");
    m_watcher->setConnection(QDBusConnection::systemBus());
    m_watcher->setWatchMode( QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration );
    connect( m_watcher, SIGNAL(serviceRegistered(QString)), this, SIGNAL(upowerAvailable()));
    connect( m_watcher, SIGNAL(serviceUnregistered(QString)), this, SIGNAL(upowerUnavailable()));
    connect( m_watcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(serviceUnregistered(QString)));
    connect( m_watcher, SIGNAL(serviceRegistered(QString)), this, SLOT(connectInterfaces()));
    connect( m_watcher, SIGNAL(serviceRegistered(QString)), this, SLOT(probeDevices()));
    
    connectInterfaces();
}

UPower::~UPower() {
    delete m_interfaceProps;
    delete m_interface;
}

bool UPower::connectInterfaces() {
    if( ! (    m_interface != 0
            && m_interfaceProps != 0
            && m_interface->isValid()
            && m_interfaceProps->isValid() ) ) {
        m_interfaceProps = new QDBusInterface(
                    "org.freedesktop.UPower", "/org/freedesktop/UPower",
                    "org.freedesktop.DBus.Properties",
                    QDBusConnection::systemBus()
                );

        if( ! m_interfaceProps->isValid() ) {
            return false;
        }

        m_interface = new QDBusInterface (
                "org.freedesktop.UPower", "/org/freedesktop/UPower",
                "org.freedesktop.UPower",
                QDBusConnection::systemBus()
               );

        if( ! m_interface->isValid() ) {
            return false;
        }

        connect( m_interface, SIGNAL(DeviceAdded(QString)), this, SLOT(deviceAdded(QString)));
        connect( m_interface, SIGNAL(DeviceRemoved(QString)), this, SIGNAL(batteryDisconnected(QString)));
        connect( m_interface, SIGNAL(Changed()), this, SLOT(changed()));
    }

    emit upowerAvailable();
    emit monitoring( tr( "UPower Available" ) );
    return true;
}

void UPower::serviceUnregistered(QString service) {
    emit monitoring( tr( "UPower Unavailable : Service %1 disconnected").arg( service ) );
}

const QString UPower::version() const {
    return m_interfaceProps->call( "Get", m_interfaceProps->interface(), "DaemonVersion" ).arguments().at(0).value<QDBusVariant>().variant().toString();
}

void UPower::probeDevices() {
    emit monitoring( tr( "Probing devices ..." ) );
    QDBusArgument argument = m_interface->call( "EnumerateDevices" ).arguments().at(0).value<QDBusArgument>();

    if( m_interface->lastError().type() == QDBusError::NoError ) {
        argument.beginArray();
        while( !argument.atEnd() ) {
            QDBusObjectPath dbusPath;
            argument >> dbusPath;
            deviceAdded( dbusPath.path() );
        }
    }
}

void UPower::deviceAdded(QString path) {
    Battery *bat = new Battery( path );
    if( bat->isValid() ) {
        connect( bat, SIGNAL(monitoring(QString)), this, SIGNAL(monitoring(QString)) );

        emit batteryDetected(bat);
        emit monitoring( tr( "Battery Added : %1" ).arg( bat->path() ) );
    } else {
        delete bat;
    }
}

void UPower::changed() {
    QSettings settings;
    settings.beginGroup("Alert");
        bool alertOnLow = settings.value( "Low", false ).toBool();
    settings.endGroup();

    if( alertOnLow ) {
        bool onLowBatteryVal = m_interfaceProps->call( "Get", m_interface->interface(), "OnLowBattery" ).arguments().at(0).value<QDBusVariant>().variant().toBool();
        if( onLowBatteryVal ) {
            emit onLowBattery();
            emit monitoring( tr( "System on low battery" ) );
        }
    }

    bool _onBattery = m_interfaceProps->call( "Get", m_interface->interface(), "OnBattery" ).arguments().at(0).value<QDBusVariant>().variant().toBool();
    if( _onBattery ) {
        emit onBattery();
        emit monitoring( tr( "System on battery" ) );
    } else {
        emit onPowerSupply();
        emit monitoring( tr( "System on power supply" ) );
    }
}

void UPower::suspend() {
    qDebug() << canSuspend();
    if( canSuspend() ) {
        m_interface->callWithCallback("Suspend", QList<QVariant>(), this, SIGNAL(suspendSucceed()));
        emit monitoring( tr( "Suspending system ...") );
    } else {
        emit suspendFail();
        emit monitoring( tr( "Suspend Failed" ) );
    }
}

void UPower::hibernate() {
    if( canHibernate() ) {
        m_interface->callWithCallback("Hibernate", QList<QVariant>(), this, SIGNAL(hibernateSucceed()));
        emit monitoring( tr( "System going into hibernation ...") );
    } else {
        emit hibernateFail();
        emit monitoring( tr( "Hibernate Failed" ) );
    }
}

void UPower::reboot() {
    emit monitoring( tr( "Rebooting ..." ) );
    QDBusInterface interface( "org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager", QDBusConnection::systemBus() );
    interface.call( "Restart" );
}

void UPower::halt() {
    emit monitoring( tr( "Stopping ..." ) );
    QDBusInterface interface( "org.freedesktop.ConsoleKit", "/org/freedesktop/ConsoleKit/Manager", "org.freedesktop.ConsoleKit.Manager", QDBusConnection::systemBus() );
    interface.call( "Stop" );
}

bool UPower::canHibernate() {
    return m_interfaceProps->call( "Get", m_interface->interface(), "CanHibernate" ).arguments().at(0).value<QDBusVariant>().variant().toBool();
}

bool UPower::canSuspend() {
    return m_interfaceProps->call( "Get", m_interface->interface(), "CanSuspend" ).arguments().at(0).value<QDBusVariant>().variant().toBool();
}
