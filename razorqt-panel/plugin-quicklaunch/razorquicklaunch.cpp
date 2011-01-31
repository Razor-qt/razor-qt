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
    int prefrerredSize = s->value("size", 32).toInt();

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
            XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(desktop);
            if (!xdg->isValid())
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not valid";
                continue;
            }
            //qDebug() << "Reading XdgDesktopFile" << xdg->localizedValue("Name").toString() << xdg->localizedValue("Genericname").toString() << xdg->localizedValue("Exec").toString();
            execname = xdg->localizedValue("Name").toString();
            QString gn(xdg->localizedValue("Genericname").toString());
            if (!gn.isEmpty())
                execname += " (" + gn + ")";
            exec = xdg->localizedValue("Exec").toString();
            icon = xdg->icon(128, QIcon());
        }
        else
        {
            execname = s->value("name", "").toString();
            exec = s->value("exec", "").toString();
            icon = QIcon(s->value("icon", "").toString());
        }

        if (icon.isNull())
        {
            qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
            continue;
        }
        QAction* tmp = new QAction(icon, execname, this);
        tmp->setData(exec);
        addAction(tmp);
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
        maxSize = qMin(size, prefrerredSize);
        maxRows = size / maxSize;
        int addon = ((count % maxRows) > 0) ? 1 : 0;
        maxColumns = count / maxRows + addon;
    }
    else
    {
        size = panel->width();
        maxSize = qMin(size, prefrerredSize);
        maxColumns = size / maxSize;
        int addon = ((count % maxColumns) > 0) ? 1 : 0;
        maxRows = count / maxColumns + addon;
    }


    int row = 0;
    int col = 0;

    foreach (QAction * a, actions())
    {
        // "icon exists" check is performed in the RazorQuickLaunch constructor
        QToolButton * btn = new QToolButton(this);
        btn->setMaximumSize(maxSize, maxSize);
        btn->setMinimumSize(maxSize, maxSize);
        btn->setIconSize(QSize(maxSize*0.6, maxSize*0.6));

        btn->setDefaultAction(a);
        btn->setToolTip(a->text());
        qDebug() << "tooltop" << a->data().toString();
        connect(btn, SIGNAL(triggered(QAction*)),
                this, SLOT(execAction(QAction*)));

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

void RazorQuickLaunch::execAction(QAction* _action)
{
    qDebug() << "RazorQuickLaunch::execAction execAction triggered with" << _action->data();
    QProcess::startDetached(_action->data().toString());
}
