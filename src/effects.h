////////////////////////////////////////
//  File      : effects.h             //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

// Composite effects

///////////////////////////////////////////////////////////////////////////

#ifndef EFFECTS_H
#define EFFECTS_H

#include "defs.h"
#include <cmath>

template<int aprec, int zprec> static inline void blurinner(unsigned char *bptr, int &zR, int &zG, int &zB, int &zA, int alpha);
template<int aprec,int zprec> static inline void blurrow(QImage &im, int line, int alpha);
template<int aprec, int zprec> static inline void blurcol(QImage &im, int col, int alpha);

///////////////////////////////////////////////////////////////////////////

class Effects : public QObject
{
    Q_OBJECT

public:
    Effects(QObject *parent=0);
    ~Effects();
    QImage shadow(QImage img);

protected:

private:

};

#endif