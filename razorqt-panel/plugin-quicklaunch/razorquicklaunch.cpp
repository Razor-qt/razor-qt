#include "razorquicklaunch.h"
#include "razor.h"
#include "razorbar.h"
#include <razorqt/xdgdesktopfile.h>


RazorPlugin* init(RazorBar* panel, QWidget* parent, const QString & name)
{
    RazorQuickLaunch * ret = new RazorQuickLaunch(panel, parent, name);
    Q_ASSERT(ret);
    return ret;
}


RazorQuickLaunch::RazorQuickLaunch(RazorBar * panel, QWidget * parent, const QString & name)
    : RazorPlugin(panel, parent, name),
      m_configId(name)
{
    cfg = new ReadSettings("quicklaunch", this);
    QSettings *s = cfg->settings();

    s->beginGroup(name);
    int preferredSize = s->value("size", 32).toInt();

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

    QGridLayout * layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    int maxRows, maxColumns, maxSize;
    count = actions().count();
    int size;
    if (panel->topbottom())
    {
        size = panel->height();
        maxSize = qMin(size, preferredSize);
        maxRows = size / maxSize;
        int addon = ((count % maxRows) > 0) ? 1 : 0;
        maxColumns = count / maxRows + addon;
    }
    else
    {
        size = panel->width();
        maxSize = qMin(size, preferredSize);
        maxColumns = size / maxSize;
        int addon = ((count % maxColumns) > 0) ? 1 : 0;
        maxRows = count / maxColumns + addon;
    }


    int row = 0;
    int col = 0;

    foreach (QAction * a, actions())
    {
        if (! qobject_cast<RazorQuickLaunchAction*>(a)->isValid())
        {
            qDebug() << "Action" << a << " is not valid. Skipping.";
            continue;
        }
        // "icon exists" check is performed in the RazorQuickLaunch constructor
        QToolButton * btn = new QToolButton(this);
        btn->setMaximumSize(maxSize, maxSize);
        btn->setMinimumSize(maxSize, maxSize);
        btn->setIconSize(QSize(maxSize*0.6, maxSize*0.6));

        btn->setDefaultAction(a);
        btn->setToolTip(a->text());

        layout->addWidget(btn, row, col);
        ++col;
        if (col >= maxColumns)
        {
            col = 0;
            ++row;
        }
    }

    setMinimumWidth(maxColumns * maxSize);
    setMinimumHeight(maxRows * maxSize);
    mainLayout()->addLayout(layout);
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
