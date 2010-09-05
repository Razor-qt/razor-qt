////////////////////////////////////////
//  File      : runner.cpp            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "runner.h"

////////////////////////////////////////

Runner::Runner(QWidget *parent) : QDialog(parent)
{
    setSizeGripEnabled(true);
    setWindowModality(Qt::WindowModal);
    read_settings();
    init();
    show();
}

Runner::~Runner()
{
}

void Runner::read_settings()
{
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Message");
    ok_button_pix_path = stl_path + style->value("ok_button_pix").toString();
    close_button_pix_path = stl_path + style->value("close_button_pix").toString();
    style->endGroup(); // Message
}

void Runner::init()
{
    load_cmd_history();

    QGridLayout *layout = new QGridLayout();
    setLayout(layout);
    QLabel *name = new QLabel(tr("RUNNER"), this);
    name->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    name->setMaximumHeight(20);
    QLabel *text = new QLabel(tr("Type command line:"), this);
    command_line = new QComboBox(this);
    command_line->setEditable(true);
    command_line->addItem(""); // add an empty entry as first
    command_line->addItems(history_list);
    command_line->setCurrentIndex(0);
    QDirModel *dir_model = new QDirModel(this);
    QCompleter *completer = new QCompleter(this);
    completer->setModel(dir_model);
    command_line->setCompleter(completer);
    QPushButton *run_but = new QPushButton(QIcon(ok_button_pix_path), tr("Run"), this);
    QPushButton *close_but = new QPushButton(QIcon(close_button_pix_path), tr("Close"), this);
    layout->addWidget(name, 0, 0, 1, 0);
    layout->addWidget(text, 1, 0, 1, 0);
    layout->addWidget(command_line, 2, 0, 1, 0);
    layout->addWidget(run_but, 3, 0);
    layout->addWidget(close_but, 3, 1);

    connect(run_but, SIGNAL(pressed()), this, SLOT(run_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
}

void Runner::load_cmd_history()
{
    // load the command history
    run_history = new QFile(QDir::tempPath() + "/antico-runner.log");
    // create it if doesn't exists
    if(!run_history->exists())
    QProcess::startDetached(QString("/bin/touch").append(" ").append(QDir::tempPath() + "/antico-runner.log"));

    if (!run_history->open(QIODevice::ReadOnly))
    {
        qCritical() << "Error on opening <antico-runner.log> file";
        return;
    }
    QTextStream read_stream(run_history);

    while (!read_stream.atEnd())
    {
         QString cmd_entry = read_stream.readLine();
         history_list << cmd_entry;
    }
    run_history->close();
}

void Runner::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Runner::mousePressEvent(QMouseEvent *event)
{
    mousepos = event->pos();
    grabMouse(QCursor(Qt::SizeAllCursor));
    raise();
}

void Runner::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Runner::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}

void Runner::run_pressed()
{
    QString cmd = command_line->currentText();

    if ((cmd != ""))
    {
        if (QProcess::startDetached(cmd))
        {
            if (!run_history->open(QIODevice::WriteOnly | QIODevice::Append))
            {
                 qCritical() << "Error on opening <antico-runner.log> file";
                 return;
            }
            QTextStream write_stream(run_history);
            write_stream << cmd << endl; // append current command at the end of file
            run_history->close();
            close();
        }

        else
        {
            Msgbox msg;
            msg.set_header(tr("COMMAND INCORRECT"));
            msg.set_info(tr("Check the command syntax.\n"
                            "If the app is not in your $PATH, type the absolute app path."));
            msg.set_icon("Critical");
            msg.exec();
        }
    }
}

void Runner::close_pressed()
{
    close();
}
