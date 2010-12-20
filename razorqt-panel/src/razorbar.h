#ifndef RAZORBAR_H
#define RAZORBAR_H
#include "defs.h"
class RazorPlugin;
class ReadSettings;
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

    enum Position
    {
        Bottom = 0,
        Top,
        Left,
        Right
    };

    RazorBar(const QString & configId);
    ~RazorBar();
    void addWidget(RazorPlugin* _widget,int _stretch, Qt::Alignment _align);

    //! Position of the panel (bottom, top, left, right). See Position.
    Position position() { return m_position; }

    /*! Returns true if is the panel horizontallo oriented (top or bottom).
        false for left or right one.
    */
    inline bool topbottom()
    {
        return m_position == Top || m_position == Bottom;
    }

public slots:
    void pluginSizeChanged();

private:
    QString m_configId;
    ReadSettings * cfg;
    //! height for top/bottom, width for left/right
    int size;
    //! width for top/bottom, height for left/right
    int sizeLimit;
    Position m_position;

    // plugin - its size
    QMap<RazorPlugin*, int> barItems;
    QBoxLayout* Layout;
};

#endif
