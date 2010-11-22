#ifndef RAZORBAR_H
#define RAZORBAR_H
#include "defs.h"
class RazorPlugin;
#include "razorplugin.h"

/**
 * @file razorbar.h
 * @brief declares Razorbar class
 */


/**
 * @brief This manages the panel (bar)
 */
class RazorBar : public QLabel
{
    Q_OBJECT
public:
    RazorBar();
    ~RazorBar();
    void addWidget(RazorPlugin* _widget,int _stretch, Qt::Alignment _align);
    void makeUp();

public slots:
    void pluginSizeChanged();

private:
    // plugin - its size
    QMap<RazorPlugin*, int> barItems;
    QHBoxLayout* Layout;
};

#endif
