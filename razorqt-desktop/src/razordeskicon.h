#ifndef RAZORDESKICON_H
#define RAZORDESKICON_H

#include <QAbstractButton>


/**
 @brief this class represents a single desktop-icon - GUI
 "Icon" is QAbstractButton based now to simplufy QSS styling
 and implementation of well-behaved minimal widgets too.
 */
class RazorDeskIcon : public QAbstractButton
{
    Q_OBJECT

public:
    /*! Construct an icons.
    \param exec QString path with application executable
    \param icon QIcon to be painted
    \param text QString Text to be displayed under the icon
    \param comment QString Text to be displayed in the tooltip
    \param position QPoint where to show the icon widget
    \param parent QWidget optional parent
    */
    RazorDeskIcon(const QString & exec,
                  const QIcon & icon,
                  const QString & text,
                  const QString & comment,
                  const QPoint & position,
                  QWidget * parent = 0);
    ~RazorDeskIcon();

    QSize sizeHint() const;

    //! \brief Set new icon position in the window
    void setPos(const QPoint & npos);

protected:
    /**
     * @brief this is used to drag-move the button and emit a new-pos event
     */
    void mouseMoveEvent(QMouseEvent* _event);
    /**
     * @brief triggered on click, resets the move variables
     */
    void mousePressEvent(QMouseEvent* _event);
    /**
     * @brief triggered on release, if we did not get moved, it was a click and we behave accordingly
     */
    void mouseReleaseEvent(QMouseEvent* _event);
    /*! \brief Custom and minimal painting for desktop icon.
     * Fine tuning design will be performed in the qss stylesheed
     */
    void paintEvent(QPaintEvent * event);

    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);

signals:
    void moved(QPoint);

private:
    //! App to launch (path)
    QString m_exec;
    bool moveMe;
    bool movedMe;
    bool firstGrab;
    bool m_mouseOver;
    QPoint firstPos;

    QPixmap * m_display;
    QPixmap * m_displayHighlight;

    void initialPainting();

private slots:
    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    void launchApp();

};

#endif

