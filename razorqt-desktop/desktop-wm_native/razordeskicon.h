/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#ifndef RAZORDESKICON_H
#define RAZORDESKICON_H

#include <QAbstractButton>
#include "desktopplugin.h"

#include <qtxdg/xdgdesktopfile.h>


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

    /*! \brief Overridden QAbstractButton::setIcon() to ensure that all
        required icons m_display and m_displayHighlight are pre-painted
        for fast and optimized paintEvent()
    */
    void setIcon(const QIcon & icon);

    //! \brief Set new icon position in the window
    void setPos(const QPoint & npos);

    void setLaunchMode(DesktopPlugin::IconLaunchMode mode) { m_launchMode = mode; }

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
    void mouseDoubleClickEvent(QMouseEvent* _event);
    /*! \brief Custom and minimal painting for desktop icon.
     * Fine tuning design will be performed in the qss stylesheed
     */
    void paintEvent(QPaintEvent * event);

    //! Set the displayed icon to its QIcon::Selected mode
    void enterEvent(QEvent * event);
    //! Set the displayed icon to its QIcon::Normal mode
    void leaveEvent(QEvent * event);

signals:
    void moved(QPoint);

private:
    bool moveMe;
    bool movedMe;
    bool firstGrab;
    bool m_mouseOver;
    QPoint firstPos;

    //! Icon for normal display (including text)
    QPixmap * m_display;
    //! Icon for mouse over
    QPixmap * m_displayHighlight;

    DesktopPlugin::IconLaunchMode m_launchMode;

    /*! \brief Paint QPixmap for given mode.
        \param mode Normal or "highlighed" mode of QIcon
        \retval a reference to QPixmap. Used for m_display and m_displayHighlight
    */
    QPixmap * initialPainting(QIcon::Mode mode);

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

