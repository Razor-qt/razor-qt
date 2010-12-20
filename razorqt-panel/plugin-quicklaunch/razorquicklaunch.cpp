#include "razorquicklaunch.h"
#include "razor.h"
#include "razorbar.h"

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
    int count = s->beginReadArray("apps");

    QString execname;
    QString exec;
    QString icon;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        execname = s->value("name", "").toString();
        exec = s->value("exec", "").toString();
        icon = s->value("icon", "").toString();
        m_icons[name] = icon;

        if (!QFile::exists(icon))
        {
            qDebug() << "Icon file" << icon << "does not exists. Skipped.";
            continue;
        }
        QAction* tmp = new QAction(QIcon(icon), execname, this);
        tmp->setData(exec);
        addAction(tmp);
    }
    s->endArray();
    s->endGroup();

    /*! \todo TODO/FIXME: here it should be decided how to
    		  organize it - horizontally/vertically. Panel
    		  supports horizontal layout only for now
     */
    QGridLayout * layout = new QGridLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    //! \todo TODO/FIXME: make it (the size) configurable?
    // now do some layouting magic to save space (rows, columns)
    int maxSize = qMin(panel->height(), 32);
    int rows = panel->height() / maxSize;
    // add 1 to columns if there is more actions in one row
    int addon = ((actions().count() % rows) > 0) ? 1 : 0;
    int cols = actions().count() / rows + addon;
    //qDebug() << actions().count() << height << maxSize << rows << cols << addon;

    int row = 0;
    int col = 0;
    foreach (QAction * a, actions())
    {
        // "icon exists" check is performed in the RazorQuickLaunch constructor
        QToolButton * btn = new QToolButton(this);
        btn->setMaximumSize(maxSize, maxSize);
        //btn->setMinimumSize(maxSize, maxSize);

        btn->setDefaultAction(a);
        btn->setToolTip(a->data().toString());
        qDebug() << "tooltop" << a->data().toString();
        connect(btn, SIGNAL(triggered(QAction*)),
                this, SLOT(execAction(QAction*)));

        layout->addWidget(btn, row, col);
        ++col;
        if (col >= cols)
        {
            col = 0;
            ++row;
        }
    }

    //setMinimumWidth(ix * 32);
    mainLayout()->addLayout(layout);
}

RazorQuickLaunch::~RazorQuickLaunch()
{
}

void RazorQuickLaunch::execAction(QAction* _action)
{
    qDebug() << "RazorQuickLaunchGUI::execAction execAction triggered with" << _action->data();
    QProcess::startDetached(_action->data().toString());
}
