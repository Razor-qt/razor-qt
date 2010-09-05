////////////////////////////////////////
//  File      : dateclock.cpp         //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "dateclock.h"

////////////////////////////////////////

Dateclock::Dateclock(QWidget *parent) : QLabel(parent)
{
    read_settings();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(timeout()));
    timer->start(5000);  // signal every 5 seconds
}

Dateclock::~Dateclock()
{
}

void Dateclock::read_settings()
{
    // get style path
    QSettings *antico = new QSettings(QDir::homePath() + "/.razor/razor.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path +stl_name, QSettings::IniFormat, this);
    style->beginGroup("Dateclock");
    date_col = style->value("date_color").value<QColor>();
    clock_col = style->value("clock_color").value<QColor>();
    style->endGroup(); //Dateclock
}

void Dateclock::timeout(void)
{
    update();
}

void Dateclock::paintEvent(QPaintEvent *)
{
    time = QTime::currentTime();
    date = QDate::currentDate();
    QPainter painter(this);
    painter.setWindow(0, 0, width(), height());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor(clock_col)); //clock color
    QFont time_font(QApplication::font().family(), 12);
    QFontMetrics time_fm(time_font);
    QString string_time = time.toString().left(5);
    int time_h = time_fm.height();
    time_font.setPixelSize(height()*8/time_h);
    painter.setFont(time_font);
    painter.drawText(0, 4, width(), height()/2, Qt::AlignCenter, string_time); //clock
    painter.setPen(QColor(date_col)); //date color
    QFont date_font (QApplication::font().family(), 12);
    QFontMetrics date_fm(date_font);
    QString string_date = date.toString("dd.MM.yy");
    int date_h = date_fm.height();
    date_font.setPixelSize(height()*5/date_h);
    painter.setFont(date_font);
    painter.drawText(0, (height()/2)+2, width(), height()/2, Qt::AlignCenter, string_date); //date
}

void Dateclock::update_style()
{
    read_settings();
    update();
}
