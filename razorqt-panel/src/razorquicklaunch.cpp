#include "razorquicklaunch.h"
#include "razor.h"


RazorQuickLaunch::RazorQuickLaunch(QString cmd, int bar): RazorPlugin(bar)
{
    cmd.remove("quicklaunch");
    //!\todo TODO/FIXME: decide if it should use its own config file
    settings = new ReadSettings("spin"+cmd+".conf");
    int stateCount = settings->getInt("count");

    gui = new RazorQuickLaunchGUI(this);

    /*!\todo TODO/FIXME: share the code with Razorspinbutton.
    		 And maybe change the configuration file syntax - it's annoying
    		 to increase the count everytime it's changed. QSettings?
    */
    for (int i = 0; i < stateCount; ++i)
    {
        QString s;
        s.setNum(i);
        QStringList Explode = settings->getString("state"+s).split("|");
        if (!QFile::exists(Explode.at(2)))
        {
            qDebug() << "Icon file" << Explode.at(2) << "does not exists. Skipped.";
            continue;
        }
        QAction* tmp = new QAction((QIcon)Explode.at(2),Explode.at(1), gui);
        tmp->setData(Explode.at(0));
        gui->addAction(tmp);
    }

    gui->setupGUI(Razor::getInstance().get_looknfeel()->getInt("razorbar_height"));
    Razor::getInstance().get_gui()->addWidget(gui, bar, 0, Qt::AlignLeft);
}

RazorQuickLaunch::~RazorQuickLaunch()
{
    //delete gui;
    delete settings;
}

bool RazorQuickLaunch::handleEvent(XEvent* _event)
{
    return RazorPlugin::handleEvent(_event);
}



RazorQuickLaunchGUI::RazorQuickLaunchGUI(RazorQuickLaunch * _owner)
        : QWidget(),
        owner(_owner)
{
    show();
}

void RazorQuickLaunchGUI::setupGUI(int height)
{
    /*! \todo TODO/FIXME: here it should be decided how to
    		  organize it - horizontally/vertically. Panel
    		  supports horizontal layout only for now
     */
    QGridLayout * layout = new QGridLayout(this);
    layout->setSpacing(0);

    //! \todo TODO/FIXME: make it (the size) configurable?
    // now do some layouting magic to save space (rows, columns)
    int maxSize = qMin(height, 32);
    int rows = height / maxSize;
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
    setLayout(layout);
}

void RazorQuickLaunchGUI::execAction(QAction* _action)
{
    qDebug() << "RazorQuickLaunchGUI::execAction execAction triggered with" << _action->data();
    QProcess::startDetached(_action->data().toString());
}


RazorQuickLaunchGUI::~RazorQuickLaunchGUI()
{
    // no need to delete anything as all is parented in QWidget (this)
    // to be deleted automagically
}
