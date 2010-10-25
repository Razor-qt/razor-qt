#ifndef RAZORLABEL_CPP
#define RAZORLABEL_CPP

#include "razorlabel.h"
/**
*@file razorlabel.cpp
*@brief implements Razorlabel
*@author Christopher "VdoP" Regali
*/


/**
*@brief does the "magic" of emitting clicked on click!
*/
void Razorlabel::mousePressEvent(QMouseEvent* _event)
{
  Q_UNUSED(_event);
  emit clicked();
}

/**
*@brief the constructor
*/
Razorlabel::Razorlabel( QWidget* _parent) : QLabel(_parent)
{

}



#endif
