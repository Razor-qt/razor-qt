/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
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


#ifndef RAZORTASKBUTTON_H
#define RAZORTASKBUTTON_H

#include <QtGui/QToolButton>
#include <QtCore/QHash>
#include <QtGui/QProxyStyle>

#include <X11/X.h>
#include <X11/Xlib.h>

class QPainter;
class QPalette;
class QMimeData;

class ElidedButtonStyle: public QProxyStyle
{
public:
    ElidedButtonStyle(QStyle* style=0): QProxyStyle(style) {}

    void drawItemText(QPainter* painter, const QRect& rect, int flags,
                      const QPalette & pal, bool enabled, const QString & text,
                      QPalette::ColorRole textRole = QPalette::NoRole ) const;
};


class RazorTaskButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RazorTaskButton(const Window window, QWidget *parent = 0);
    virtual ~RazorTaskButton();

    bool isAppHidden() const;
    bool isApplicationActive() const;
    Window windowId() const { return mWindow; }

    QSize   sizeHint() const;
    static void unCheckAll();
    int desktopNum() const;

    void updateText();
    void updateIcon();

    static void setShowOnlyCurrentDesktopTasks(bool);

public slots:
    void raiseApplication();
    void minimizeApplication();
    void maximizeApplication();
    void deMaximizeApplication();
    void shadeApplication();
    void unShadeApplication();
    void closeApplication();
    void moveApplicationToDesktop();
    void setApplicationLayer();

    void handlePropertyNotify(XPropertyEvent* event);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);

    void nextCheckState();
    void contextMenuEvent( QContextMenuEvent* event);

private:
    Window mWindow;
    static RazorTaskButton* mCheckedBtn;
    ElidedButtonStyle mStyle;
    const QMimeData *mDraggableMimeData;
    static bool mShowOnlyCurrentDesktopTasks;

private slots:
    void btnClicked(bool checked);
    void checkedChanged(bool checked);
    void activateWithDraggable();
};

typedef QHash<Window,RazorTaskButton*> RazorTaskButtonHash;

#endif // RAZORTASKBUTTON_H
