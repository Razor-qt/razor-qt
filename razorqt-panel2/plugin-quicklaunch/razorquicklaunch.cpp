#include "razorquicklaunch.h"
#include <razorqt/xdgdesktopfile.h>
#include "razorqt/readsettings.h"

#include <QtDebug>
#include <QProcess>


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorQuickLaunch)


RazorQuickLaunch::RazorQuickLaunch(RazorPanel* panel, const QString& configId, QWidget *parent)
    : RazorPanelPlugin(panel, configId, parent),
      m_configId(configId)
{
    setObjectName("QuickLaunch");
    setWindowTitle(tr("Quick Launcher"));

    cfg = new ReadSettings("quicklaunch", this);
    QSettings *s = cfg->settings();

    s->beginGroup(configId);

    int count = s->beginReadArray("apps");

    QString desktop;
    QString execname;
    QString exec;
    QIcon icon;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        desktop = s->value("desktop", "").toString();
        if (! desktop.isEmpty())
        {
            addAction(new RazorQuickLaunchAction(desktop, this));
        }
        else
        {
            execname = s->value("name", "").toString();
            exec = s->value("exec", "").toString();
            icon = QIcon(s->value("icon", "").toString());
            if (icon.isNull())
            {
                qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
                continue;
            }
            addAction(new RazorQuickLaunchAction(execname, exec, icon, this));
        }

    }

    s->endArray();
    s->endGroup();
}

RazorQuickLaunch::~RazorQuickLaunch()
{
}

RazorQuickLaunchAction::RazorQuickLaunchAction(const QString & name,
                                               const QString & exec,
                                               const QIcon & icon,
                                               QWidget * parent)
    : QAction(icon, name, parent),
      m_valid(true)
{
    m_type = ActionLegacy;

    setData(exec);
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

RazorQuickLaunchAction::RazorQuickLaunchAction(const QString & desktop,
                                               QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionXdg;

    XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(desktop);
    if (!xdg->isValid())
    {
        qDebug() << "XdgDesktopFile" << desktop << "is not valid";
        m_valid = false;
        return;
    }

    QString title(xdg->localizedValue("Name").toString());
    QString gn(xdg->localizedValue("GenericName").toString());
    if (!gn.isEmpty())
        title += " (" + gn + ")";
    setText(title);

    setIcon(xdg->icon(128, QIcon()));

    setData(desktop);
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

void RazorQuickLaunchAction::execAction()
{
    QString exec(data().toString());
    switch (m_type)
    {
        case ActionLegacy:
            QProcess::startDetached(exec);
            break;
        case ActionXdg:
        {
            XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(exec);
            if (xdg->isValid())
                xdg->startDetached();
            break;
        }
    }
}
