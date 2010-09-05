////////////////////////////////////////
//  File      : colorsel.h            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// widget for color generation

////////////////////////////////////////

#ifndef COLORSEL_H
#define COLORSEL_H

#include "defs.h"

////////////////////////////////////////

class Colorsel : public QWidget
{
    Q_OBJECT

public:
    Colorsel(const QColor &, QWidget *parent = 0);
    ~Colorsel();
    QSize minimumSizeHint() const;
    QColor get_color() const;

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintBorder();
    void paintBar();
    void paintValue();
    void paintText();

private:
    double redY;
    double greenY;
    double blueY;
    double opY;
    double redyp;
    double greenyp;
    double blueyp;
    double opyp;
};

#endif
