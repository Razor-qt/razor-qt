#ifndef RAZORMODMAN_CPP
#define RAZORMODMAN_CPP
#include "razormodman.h"
/**
 * @file razormodman.cpp
 * @author Christopher "VdoP" Regali
 * @brief implements class Razormodulemanager
 */


/**
 * @brief the constructor, needs a valid modules.conf
 */
RazorModuleManager::RazorModuleManager(QObject* parent)
    : QObject(parent)
{
    stateMan = new RazorState;
    cfg = new ReadSettings("session", this);

    QSettings * s = cfg->settings();
    autorestart = s->value("autorestart", true).toBool();

    int count = s->beginReadArray("modules");
    QString cmd;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        cmd = s->value("exec", "").toString();
        if (cmd.isEmpty())
        {
            qDebug() << __FILE__ << ":" << __LINE__ << "empty name for module. Skipping.";
            continue;
        }
        bool power = s->value("doespower", false).toBool();

        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        if (power)
            stateMan->addProcess(tmp);

        connect(tmp, SIGNAL(finished(int, QProcess::ExitStatus)),
                this,SLOT(restartModules(int, QProcess::ExitStatus)));
        Module m;
        m.power = power;
        m.process = tmp;
        procMap[cmd] = m;
    }
    s->endArray();
    
    int delay = s->value("autostart_delay", 1).toInt();
    QTimer::singleShot(delay * 1000, this, SLOT(autoStartSingleShot()));
}

void RazorModuleManager::autoStartSingleShot()
{
    QSettings * s = cfg->settings();
    int count = s->beginReadArray("autostart");
    QString cmd;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        cmd = s->value("exec", "").toString();
        if (cmd.isEmpty())
        {
            qDebug() << __FILE__ << ":" << __LINE__ << "empty name for module. Skipping.";
            continue;
        }
        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        autostartList << tmp;
    }
    s->endArray();
}

/**
 * @brief this slot is called by the QProcesses if they end - they should NOT! so they get restarted here
 */
void RazorModuleManager::restartModules(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "void RazorModuleManager::restartModules() called and it's wrong. Something is failing";
    QProcess * proc = qobject_cast<QProcess*>(sender());
    Q_ASSERT(proc);
    QString procName;
    bool power = false;

    ModulesMapIterator i(procMap);
    while (i.hasNext()) {
        i.next();
        if (i.value().process == proc)
        {
            procName = i.key();
            power = i.value().power;
            break;
        }
    }
    
    switch (exitStatus)
    {
        case QProcess::NormalExit:
            if (! power)
            {
                // autostart
                foreach (QProcess * p, autostartList)
                {
                    p->blockSignals(true);
                    p->terminate();
                    if (!p->waitForFinished())
                        qDebug() << "Autostart" << p << "rejected to close correctly. Kill it down.";
                    delete p;
                }

                // modules
                ModulesMapIterator i(procMap);
                while (i.hasNext())
                {
                    i.next();
                    QProcess * p = i.value().process;
                    p->blockSignals(true);
                    p->terminate();
                    if (!p->waitForFinished())
                        qDebug() << "Autostart" << p << "rejected to close correctly. Kill it down.";
                }
		// WTF?! why it does not do all I need?
		// I think I simulate RAZOR_DO_LOGOUT above...
                stateMan->doOperation("RAZOR_DO_LOGOUT");
		QCoreApplication::exit(0);
            }
            break;
        case QProcess::CrashExit:
            if (autorestart)
            {
                qDebug() << "Process" << procName << "(" << proc << ") has to be restarted";
                proc->start(procName);
            }
            break;
    }

}



/**
 * @brief the destructor, cleans up our mess
 */
RazorModuleManager::~RazorModuleManager()
{
//    ModulesMapIterator i(procMap);
//    while (i.hasNext())
//    {
//        i.next();
//        QProcess * p = i.value().process;
//        if (!p->waitForFinished())
//            qDebug() << "Module" << p << "rejected to close correctly. Kill it down.";
//        delete p;
//    }
    // autostart
//    foreach (QProcess * p, autostartList)
//    {
//        p->terminate();
//        if (!p->waitForFinished())
//            qDebug() << "Autostart" << p << "rejected to close correctly. Kill it down.";
//        delete p;
//    }
    //
    delete stateMan;
}





#endif
