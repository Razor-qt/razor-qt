#ifndef RAZORBAR_H
#define RAZORBAR_H
#include "defs.h"
/**
 * @file razorbar.h
 * @brief declares Razorbar class
 */


/**
 * @brief This manages the panel (bar)
 */
class Razorbar : public QLabel
{
    Q_OBJECT
public:
    Razorbar();
    ~Razorbar();
    void addWidget(QWidget* _widget,int _stretch, Qt::Alignment _align);
    void makeUp();
private:
    QList<QWidget*> barItems;
    QHBoxLayout* Layout;
};

#endif
