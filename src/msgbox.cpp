////////////////////////////////////////
//  File      : msgbox.cpp            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "msgbox.h"

////////////////////////////////////////

Msgbox::Msgbox(QWidget *parent) : QDialog(parent)
{
    setSizeGripEnabled(true);
    read_settings();
    init();
}

Msgbox::~Msgbox()
{}

void Msgbox::init()
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    msg_header = new QLabel(this);
    msg_header->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    msg_header->setMaximumHeight(20);
    msg_info = new QLabel(this);
    msg_icon = new QLabel(this);
    button_box = new QDialogButtonBox(this);
    ok_but = new QPushButton(QIcon(ok_button_pix_path), tr("Ok"), this);
    close_but = new QPushButton(QIcon(close_button_pix_path), tr("Close"), this);
    layout->addWidget(msg_header, 0, 0, 1, 0);
    layout->addWidget(msg_icon, 1, 0, Qt::AlignCenter);
    layout->addWidget(msg_info, 1, 1, Qt::AlignCenter);
    layout->addWidget(button_box, 2, 0, 1, 0);

    connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
}

void Msgbox::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Msgbox::read_settings()
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
    add_button_pix_path = stl_path + style->value("add_button_pix").toString();
    remove_button_pix_path = stl_path + style->value("remove_button_pix").toString();
    question_pix_path = stl_path + style->value("question_pix").toString();
    information_pix_path = stl_path + style->value("information_pix").toString();
    warning_pix_path = stl_path + style->value("warning_pix").toString();
    critical_pix_path = stl_path + style->value("critical_pix").toString();
    style->endGroup(); // Message
}

void Msgbox::set_header(const QString &header)
{
    msg_header->setText(header);
}

void Msgbox::set_info(const QString &info)
{
    msg_info->setText(info);
}

void Msgbox::set_icon(const QString &type)
{
    if (type == "Information")
    {
        msg_icon->setPixmap(QPixmap(information_pix_path).scaled(48, 48));
        button_box->removeButton(ok_but);
        button_box->addButton(close_but, QDialogButtonBox::RejectRole);
    }
    if (type == "Warning")
    {
        msg_icon->setPixmap(QPixmap(warning_pix_path).scaled(48, 48));
        button_box->removeButton(ok_but);
        button_box->addButton(close_but, QDialogButtonBox::RejectRole);
    }
    if (type == "Critical")
    {
        msg_icon->setPixmap(QPixmap(critical_pix_path).scaled(48, 48));
        button_box->removeButton(ok_but);
        button_box->addButton(close_but, QDialogButtonBox::RejectRole);
    }
    if (type == "Question")
    {
        msg_icon->setPixmap(QPixmap(question_pix_path).scaled(48, 48));
        button_box->addButton(ok_but, QDialogButtonBox::AcceptRole);
        button_box->addButton(close_but, QDialogButtonBox::RejectRole);
    }
}

void Msgbox::mousePressEvent(QMouseEvent *event)
{
    mousepos = event->pos();
    grabMouse(QCursor(Qt::SizeAllCursor));
    raise();
}

void Msgbox::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Msgbox::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}


