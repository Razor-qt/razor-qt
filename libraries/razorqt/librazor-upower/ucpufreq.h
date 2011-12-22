/*
 * (c)GPL3
This is probably temporary code probably. We will merge it into potential shared library.

This code is based on cool Brieuc ROBLIN's pyrotools.org applets, license GPLv3.
*/

#ifndef UCPUFREQ_HXX
#define UCPUFREQ_HXX

#include <QObject>
#include <QDBusInterface>
#include <QDBusServiceWatcher>
#include <QStringList>

class UCpufreq : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "author", "Brieuc ROBLIN")
    Q_CLASSINFO( "url", "http://pt.pyrotools.org/")

public:
    UCpufreq();
    ~UCpufreq();

    bool isValid() const;
    int nbCpus() const;
    QString governor( unsigned long cpu ) const;
    unsigned long frequency( unsigned long cpu ) const;

public Q_SLOTS:
    void probeCpus();
    bool createInterface();

    bool setFrequency( qulonglong cpu, qulonglong f );
    bool setGovernor( qulonglong cpu, QString g );
    bool setMinFrequency( qulonglong cpu, qulonglong f );
    bool setMaxFrequency( qulonglong cpu, qulonglong f );

    QList<unsigned long> availableFrequency() const;
    QStringList availableGovernors() const;
    QList<unsigned long> relatedCpus( unsigned long cpu ) const;

private:
    QDBusInterface *m_interface;
    QList<QDBusInterface*> *m_cpuinterfaces;
    QDBusServiceWatcher *m_watcher;

private Q_SLOTS:
    void clearInterfaces();

Q_SIGNALS:
    void frequencyChange( uint cpu );
    void maxFrequencyChange( uint cpu );
    void minFrequencyChange( uint cpu );
    void governorChange( uint cpu );

    void frequencyChange();
    void maxFrequencyChange();
    void minFrequencyChange();
    void governorChange();

    void ucpufreqAvailable();
    void ucpufreqUnavailable();

    void cpuAvailable();

    void monitoring( QString );

};

#endif // UCPUFREQ_HXX
