////////////////////////////////////////
//  File      : colorsel.cpp          //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "colorsel.h"

////////////////////////////////////////

Colorsel::Colorsel(const QColor &color, QWidget *parent) : QWidget(parent)
{
    redY = color.red();
    greenY = color.green();
    blueY = color.blue();
    opY = color.alpha();
    redyp = redY/2;
    greenyp = greenY/2;
    blueyp = blueY/2;
    opyp = opY/2;
}

Colorsel::~Colorsel()
{}

void Colorsel::paintEvent(QPaintEvent *)
{
    paintBorder();
    paintBar();
    paintValue();
    paintText();
}

void Colorsel::paintBorder()
{
    QPainter painter(this);
    painter.setWindow(0, 0, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(80, 80, 80), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    QLinearGradient linGrad1(10, 100, 15, 100);
    linGrad1.setColorAt(0, Qt::white);
    linGrad1.setColorAt(1, QColor(80, 80, 80));
    linGrad1.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad1);

    QRectF border(10, 10, 180, 180);
    painter.drawRect(border);

    QLinearGradient linGrad2(190, 100, 195, 100);
    linGrad2.setColorAt(0, QColor(80, 80, 80));
    linGrad2.setColorAt(1, Qt::black);
    linGrad2.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad2);

    QRectF border1(15, 10, 180, 180);
    painter.drawRect(border1);
}

void Colorsel::paintBar()
{
    QPainter painter(this);
    painter.setWindow(0, 0, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::black);

    QLinearGradient linGrad1(30, 90, 35, 90);
    linGrad1.setColorAt(0, Qt::gray);
    linGrad1.setColorAt(1, Qt::black);
    linGrad1.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad1);
    QRectF gradRect1(30, 30, 20, 127.5);
    painter.drawRect(gradRect1);

    QLinearGradient redGrad(40, 142.5, 40, 30);
    redGrad.setColorAt(0, Qt::red);
    redGrad.setColorAt(1, Qt::black);
    redGrad.setSpread(QGradient::PadSpread);
    painter.setOpacity(0.75);
    painter.setBrush(redGrad);
    QRectF red(30, 30, 20, redyp);
    painter.drawRect(red);

    QLinearGradient linGrad2(60, 90, 65, 90);
    linGrad2.setColorAt(0, Qt::gray);
    linGrad2.setColorAt(1, Qt::black);
    linGrad2.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad2);
    QRectF gradRect2(60, 30, 20, 127.5);
    painter.drawRect(gradRect2);

    QLinearGradient greenGrad(70, 142.5, 70, 30);
    greenGrad.setColorAt(0, Qt::green);
    greenGrad.setColorAt(1, Qt::black);
    greenGrad.setSpread(QGradient::PadSpread);
    painter.setOpacity(0.75);
    painter.setBrush(greenGrad);
    QRectF green(60, 30, 20, greenyp);
    painter.drawRect(green);

    QLinearGradient linGrad3(90, 90, 95, 90);
    linGrad3.setColorAt(0, Qt::gray);
    linGrad3.setColorAt(1, Qt::black);
    linGrad3.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad3);
    QRectF gradRect3(90, 30, 20, 127.5);
    painter.drawRect(gradRect3);

    QLinearGradient blueGrad(100, 142.5, 100, 30);
    blueGrad.setColorAt(0, Qt::blue);
    blueGrad.setColorAt(1, Qt::black);
    blueGrad.setSpread(QGradient::PadSpread);
    painter.setOpacity(0.75);
    painter.setBrush(blueGrad);
    QRectF blue(90, 30, 20, blueyp);
    painter.drawRect(blue);

    QLinearGradient linGrad4(120, 90, 125, 90);
    linGrad4.setColorAt(0, Qt::gray);
    linGrad4.setColorAt(1, Qt::black);
    linGrad4.setSpread(QGradient::PadSpread);
    painter.setBrush(linGrad4);
    QRectF gradRect4(120, 30, 20, 127.5);
    painter.drawRect(gradRect4);

    QLinearGradient opGrad(130, 142.5, 130, 30);
    opGrad.setColorAt(0, Qt::white);
    opGrad.setColorAt(1, Qt::black);
    opGrad.setSpread(QGradient::PadSpread);
    painter.setOpacity(0.75);
    painter.setBrush(opGrad);
    QRectF op(120, 30, 20, opyp);
    painter.drawRect(op);

    painter.setOpacity(1);
    QColor color(redY, greenY, blueY, opY);
    painter.setBrush(color);
    QRectF tot(150, 30, 30, 150.5);
    painter.drawRect(tot);

}

void Colorsel::paintValue()
{
    QPainter painter(this);
    painter.setWindow(0, 0, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(Qt::black));

    QRectF redRect(30, 165, 20, 15);
    painter.drawRect(redRect);

    QRectF greenRect(60, 165, 20, 15);
    painter.drawRect(greenRect);

    QRectF blueRect(90, 165, 20, 15);
    painter.drawRect(blueRect);

    QRectF opRect(120, 165, 20, 15);
    painter.drawRect(opRect);

    QFont valFont("Sans Serif", 7, QFont::Bold);
    painter.setFont(valFont);
    painter.setPen(Qt::yellow);

    QString redVal = QString("%1").arg(redY,0,'f',0);
    painter.drawText(redRect, Qt::AlignCenter, redVal);

    QString greenVal = QString("%1").arg(greenY,0,'f',0);
    painter.drawText(greenRect, Qt::AlignCenter, greenVal);

    QString blueVal = QString("%1").arg(blueY,0,'f',0);
    painter.drawText(blueRect, Qt::AlignCenter, blueVal);

    QString opVal = QString("%1").arg(opY,0,'f',0);
    painter.drawText(opRect, Qt::AlignCenter, opVal);
}

void Colorsel::paintText()
{
    QPainter painter(this);
    painter.setWindow(0, 0, 200, 200);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF R(30, 15, 20, 15);
    QRectF G(60, 15, 20, 15);
    QRectF B(90, 15, 20, 15);
    QRectF A(120, 15, 20, 15);
    painter.setPen(QPen(Qt::white));
    QFont labFont("Sans Serif", 8, QFont::Normal);
    painter.setFont(labFont);
    painter.drawText(R, Qt::AlignCenter, "R");
    painter.drawText(G, Qt::AlignCenter, "G");
    painter.drawText(B, Qt::AlignCenter, "B");
    painter.drawText(A, Qt::AlignCenter, "A");
}

void Colorsel::mousePressEvent(QMouseEvent *e)
{
    setCursor(QCursor(Qt::SizeVerCursor));

    double x = e->x();
    double y = e->y();

    double wrun = width();
    double hrun = height();

    double wstart = 200;
    double hstart = 200;

    double dx = wrun/wstart;
    double dy = hrun/hstart;

    double diff = 60;
    double diff2 = 30;
    double coeff = 2;

    double py = ((y/dy)*coeff)-diff;
    double pyp = (y/dy)-diff2;

    if (py >= 254)
        py = 255;
    if (py <= 2)
        py = 0;

    if (y>=(30*dy) && y<=(157.5*dy))
    {
        if (x>=(30*dx) && x<=(50*dx))
        {
            redY = py;
            redyp = pyp;
        }
        if (x>=(60*dx) && x<=(80*dx))
        {
            greenY = py;
            greenyp = pyp;
        }
        if (x>=(90*dx) && x<=(110*dx))
        {
            blueY = py;
            blueyp = pyp;
        }
        if (x>=(120*dx) && x<=(140*dx))
        {
            opY = py;
            opyp = pyp;
        }
        update();
    }
}

void Colorsel::mouseMoveEvent(QMouseEvent *e)
{
    double x = e->x();
    double y = e->y();

    double wrun = width();
    double hrun = height();

    double wstart = 200;
    double hstart = 200;

    double dx = wrun/wstart;
    double dy = hrun/hstart;

    double diff = 60;
    double diff2 = 30;
    double coeff = 2;

    double py = ((y/dy)*coeff)-diff;
    double pyp = (y/dy)-diff2;

    if (py >= 254)
        py = 255;
    if (py <= 2)
        py = 0;

    if (y>=(30*dy) && y<=(157.5*dy))
    {
        if (x>=(30*dx) && x<=(50*dx))
        {
            redY = py;
            redyp = pyp;
        }
        if (x>=(60*dx) && x<=(80*dx))
        {
            greenY = py;
            greenyp = pyp;
        }
        if (x>=(90*dx) && x<=(110*dx))
        {
            blueY = py;
            blueyp = pyp;
        }
        if (x>=(120*dx) && x<=(140*dx))
        {
            opY = py;
            opyp = pyp;
        }
        update();
    }
}

void Colorsel::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setCursor(QCursor(Qt::ArrowCursor));
}

QSize Colorsel::minimumSizeHint() const
{
    return QSize(20, 20);
}

QColor Colorsel::get_color() const
{
    QColor col(redY, greenY, blueY, opY);
    return col;
}
