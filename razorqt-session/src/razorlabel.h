#ifndef RAZORLABEL_H
#define RAZORLABEL_H
#include <QString>
#include <QLabel>
#include <QWidget>


/**
*@file razorlabel.h
*@brief declares Razorlabel
*@author Christopher "VdoP" Regali
*/


/**
* @brief does emit a clicked() event on click - thats the difference!
*/
class Razorlabel : public QLabel
{
    Q_OBJECT
public:
    Razorlabel( QWidget* _parent=0);
    ~Razorlabel() {}
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* _event);
};


#endif
