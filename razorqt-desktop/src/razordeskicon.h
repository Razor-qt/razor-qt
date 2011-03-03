#ifndef RAZORDESKICON_H
#define RAZORDESKICON_H

#include <QAbstractButton>

#include <razorqt/xdgdesktopfile.h>


/*! \brief Abstract base for all desktop icons.
It does all shared tasks like painting, moving, mouse action catching...
"Icon" is QAbstractButton based now to simplufy QSS styling
and implementation of well-behaved minimal widgets too.
*/
class RazorDeskIconBase : public QAbstractButton
{
    Q_OBJECT

public:
    /*! Construct an icons.
    \param position QPoint where to show the icon widget
    \param parent QWidget optional parent
    */
    RazorDeskIconBase(const QPoint & position,
                      QWidget * parent = 0);
    ~RazorDeskIconBase();

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
    virtual void launchApp() = 0;

};


/** \brief Icon for the XDG desktop file. It uses XdgDesktopFile handler
from librazorqt.
 */
class RazorDeskIconDesktop : public RazorDeskIconBase
{
    Q_OBJECT
    
public:
    RazorDeskIconDesktop(XdgDesktopFile * xdg,
                         const QPoint & position,
                         QWidget * parent = 0
                        );
    ~RazorDeskIconDesktop();

private:
    XdgDesktopFile * m_xdg;

private slots:
    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    void launchApp();

};

/** \brief Icon for standard file. It uses QDesktopServices to launch the
"open with" application by file mime type.
 */
class RazorDeskIconFile : public RazorDeskIconBase
{
    Q_OBJECT
    
public:
    RazorDeskIconFile(const QString & file,
                      const QPoint & position,
                      QWidget * parent = 0
                     );

private:
    QString m_file;

private slots:
    /**
    @brief Launch assigned m_exec binary/path.
    Called when is the button clicked.
     */
    void launchApp();

};

#endif

