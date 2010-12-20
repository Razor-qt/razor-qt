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
        bool power = s->value("power", false).toBool();

        QProcess* tmp = new QProcess(this);
        tmp->start(cmd);
        if (power)
            stateMan->addProcess(tmp);

        if (autorestart)
            connect(tmp, SIGNAL(finished(int)),this,SLOT(restartModules()));
        Module m;
        m.power = power;
        m.process = tmp;
        procMap[cmd] = m;
    }
    s->endArray();
}

/**
 * @brief this slot is called by the QProcesses if they end - they should NOT! so they get restarted here
 */
void RazorModuleManager::restartModules()
{
    qDebug() << "void Razormodulemanager::restartModules() called and it's wrong. Something is failing";

    ModulesMapIterator i(procMap);
    while (i.hasNext()) {
        i.next();
        if (i.value().process->state() == QProcess::NotRunning)
        {
            qDebug() << i.key() << "has been restarted";
            i.value().process->start(i.key());
        }
    }
}


/**
 * @brief the destructor, cleans up our mess
 */
RazorModuleManager::~RazorModuleManager()
{
    QSettings * s = cfg->settings();
    s->setValue("autorestart", autorestart);
    s->beginWriteArray("modules");
    ModulesMapIterator i(procMap);
    int ix = 0;
    while (i.hasNext()) {
        i.next();
        s->setArrayIndex(ix);
        s->setValue("exec_name", i.key());
        s->setValue("power", i.value().power);
        ++ix;
        delete i.value().process;
    }
    //
    delete stateMan;
}





#endif
