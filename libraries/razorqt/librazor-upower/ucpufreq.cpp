/*
 * (c)GPL3
This is probably temporary code probably. We will merge it into potential shared library.

This code is based on cool Brieuc ROBLIN's pyrotools.org applets, license GPLv3.
*/

#include "ucpufreq.h"

#include <QtDBus>

UCpufreq::UCpufreq() : QObject(), m_interface(0) {
    m_cpuinterfaces = new QList<QDBusInterface*>;

    m_watcher = new QDBusServiceWatcher;
    m_watcher->addWatchedService("org.pyrotools.UCpufreq");
    m_watcher->setConnection(QDBusConnection::systemBus());
    m_watcher->setWatchMode( QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration );
    connect( m_watcher, SIGNAL(serviceUnregistered(QString)), this, SIGNAL(ucpufreqUnavailable()));
    connect( m_watcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(clearInterfaces()));
    connect( m_watcher, SIGNAL(serviceRegistered(QString)), this, SLOT(createInterface()));
}

UCpufreq::~UCpufreq() {
    foreach( QDBusInterface *t, *m_cpuinterfaces ) {
        t->deleteLater();
    }
    delete m_cpuinterfaces;
}

bool UCpufreq::createInterface() {
    if(    m_interface != 0
           && m_interface->isValid() ) {

        connect( m_interface, SIGNAL(cpusFound()), this, SLOT(probeCpus()));

        emit ucpufreqAvailable();

        if( isValid() ) {
            probeCpus();
        }

        return true;
    } else {
        m_interface = new QDBusInterface (
                "org.pyrotools.UCpufreq", "/org/pyrotools/UCpufreq",
                "org.pyrotools.UCpufreq",
                QDBusConnection::systemBus()
            );
        if( ! m_interface->isValid() ) {
            return false;
        }
        connect( m_interface, SIGNAL(cpusFound()), this, SLOT(probeCpus()));

        emit ucpufreqAvailable();

        if( isValid() ) {
            probeCpus();
        }

        return true;
    }
}

void UCpufreq::probeCpus() {
    clearInterfaces();

    QDBusArgument argument = m_interface->call( "cpusAvailable" ).arguments().at(0).value<QDBusArgument>();

    if( m_interface->lastError().type() == QDBusError::NoError ) {
        argument.beginArray();
        while( !argument.atEnd() ) {
            QDBusObjectPath dbusPath;
            argument >> dbusPath;
            QDBusInterface *cpuinterface = new QDBusInterface(
                    "org.pyrotools.UCpufreq", dbusPath.path(),
                    "org.pyrotools.UCpufreq.cpu",
                    QDBusConnection::systemBus()
                );
            if( cpuinterface->isValid() ) {
                m_cpuinterfaces->append(cpuinterface);
                connect( cpuinterface, SIGNAL(governorChange(QString)), this, SIGNAL(governorChange()));
                connect( cpuinterface, SIGNAL(frequencyChange(qulonglong)), this, SIGNAL(frequencyChange()));
                connect( cpuinterface, SIGNAL(maxFrequencyChange(qulonglong)), this, SIGNAL(maxFrequencyChange()));
                connect( cpuinterface, SIGNAL(minFrequencyChange(qulonglong)), this, SIGNAL(minFrequencyChange()));
            }
        }
    }

    emit cpuAvailable();
}

int UCpufreq::nbCpus() const {
    return m_cpuinterfaces->size();
}

void UCpufreq::clearInterfaces() {
    foreach( QDBusInterface *t, *m_cpuinterfaces ) {
        t->deleteLater();
    }
    m_cpuinterfaces->clear();
}

bool UCpufreq::setFrequency(qulonglong cpu, qulonglong f) {
    if( cpu == (qulonglong) -1 ) {
        foreach( QDBusInterface *i, *m_cpuinterfaces ) {
            i->call("setFrequency", f);
        }
    } else {
        m_cpuinterfaces->at(cpu)->call("setFrequency", f);
    }
    return false;
}

bool UCpufreq::setGovernor(qulonglong cpu, QString g) {
    if( cpu == (qulonglong) -1 ) {
        foreach( QDBusInterface *i, *m_cpuinterfaces ) {
            i->call("setGovernor", g);
        }
    } else {
        m_cpuinterfaces->at(cpu)->call("setGovernor", g);
    }
    return false;
}

bool UCpufreq::setMaxFrequency(qulonglong cpu, qulonglong f) {
    Q_UNUSED( cpu )
    Q_UNUSED( f )
    return false;
}

bool UCpufreq::setMinFrequency(qulonglong cpu, qulonglong f) {
    Q_UNUSED( cpu )
    Q_UNUSED( f )
    return false;
}

QList<unsigned long> UCpufreq::availableFrequency() const {
    QList<unsigned long> list;
    if( m_cpuinterfaces->size() > 0  ) {
        QDBusArgument argument = m_cpuinterfaces->at(0)->call("frequencies").arguments().at(0).value<QDBusArgument>();
        if( m_cpuinterfaces->at(0)->lastError().type() == QDBusError::NoError ) {
            argument.beginArray();
            while( !argument.atEnd() ) {
                QVariant var;
                argument >> var;
                list.append( var.toLongLong() );
            }
        }
    }
    return list;
}

QList<unsigned long> UCpufreq::relatedCpus(unsigned long cpu) const {
    QList<unsigned long> list;
    if( m_cpuinterfaces->size() > 0  ) {
        QDBusArgument argument = m_cpuinterfaces->at(cpu)->call("relatedCpus").arguments().at(0).value<QDBusArgument>();
        if( m_cpuinterfaces->at(0)->lastError().type() == QDBusError::NoError ) {
            argument.beginArray();
            while( !argument.atEnd() ) {
                QVariant var;
                argument >> var;
                list.append( var.toLongLong() );
            }
        }
    }
    return list;
}

QStringList UCpufreq::availableGovernors() const {
    if( m_cpuinterfaces->size() > 0 ) {
        return m_cpuinterfaces->at(0)->call("governors").arguments().at(0).toStringList();
    }
    return QStringList();
}

QString UCpufreq::governor( unsigned long cpu = 0 ) const {
    if( (unsigned long) m_cpuinterfaces->size() > cpu ) {
        return m_cpuinterfaces->at(cpu)->call("governor").arguments().at(0).toString();
    }
    return QString();
}

unsigned long UCpufreq::frequency( unsigned long cpu = 0 ) const {
    if( (unsigned long) m_cpuinterfaces->size() > cpu ) {
        unsigned long hw = m_cpuinterfaces->at(cpu)->call("hardwareFrequency").arguments().at(0).toLongLong();
        unsigned long kernel = m_cpuinterfaces->at(cpu)->call("kernelFrequency").arguments().at(0).toLongLong();
        if( hw != 0 ) {
            return hw;
        }
        return kernel;
    }
    return 0;
}

bool UCpufreq::isValid() const {
    return m_interface != 0
            && m_interface->isValid() ;
}
