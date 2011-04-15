#include "razorquicklaunch.h"
#include <razorqt/xdgdesktopfile.h>
#include "razorqt/xdgicon.h"

#include <QtDebug>
#include <QProcess>
#include <QToolButton>
#include <QtCore/QSettings>

EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorQuickLaunch)


RazorQuickLaunch::RazorQuickLaunch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent)
{
    setObjectName("QuickLaunch");

    int count = settings().beginReadArray("apps");

    QString desktop;
    QString execname;
    QString exec;
    QIcon icon;
    for (int i = 0; i < count; ++i)
    {
        settings().setArrayIndex(i);
        desktop = settings().value("desktop", "").toString();
        if (! desktop.isEmpty())
        {
            XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(desktop);
            if (!xdg->isValid())
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not valid";
                continue;
            }
            addButton(new RazorQuickLaunchAction(xdg, this));
        }
        else
        {
            execname = settings().value("name", "").toString();
            exec = settings().value("exec", "").toString();
            icon = QIcon(settings().value("icon", "").toString());
            if (icon.isNull())
            {
                qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
                continue;
            }
            addButton(new RazorQuickLaunchAction(execname, exec, icon, this));
        }

    }

    settings().endArray();
}

RazorQuickLaunch::~RazorQuickLaunch()
{
}

void RazorQuickLaunch::addButton(QAction* action)
{
    QToolButton* btn = new QToolButton(this);
    btn->setDefaultAction(action);
    btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn->setIconSize(QSize(22, 22));
    addWidget(btn);
}

RazorQuickLaunchAction::RazorQuickLaunchAction(const QString & name,
                                               const QString & exec,
                                               const QIcon & icon,
                                               QWidget * parent)
    : QAction(icon, name, parent),
      m_valid(true)
{
    m_type = ActionLegacy;

    if (icon.isNull())
        setIcon(XdgIcon::defaultApplicationIcon());

    setData(exec);
    connect(this, SIGNAL(triggered()), this, SLOT(execAction()));
}

RazorQuickLaunchAction::RazorQuickLaunchAction(const XdgDesktopFile * xdg,
                                               QWidget * parent)
    : QAction(parent),
      m_valid(true)
{
    m_type = ActionXdg;

    QString title(xdg->localizedValue("Name").toString());
    QString gn(xdg->localizedValue("GenericName").toString());
    if (!gn.isEmpty())
        title += " (" + gn + ")";
    setText(title);

    setIcon(xdg->icon(128, XdgIcon::defaultApplicationIcon()));

    setData(xdg->fileName());
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
