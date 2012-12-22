/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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


#ifndef PLUGINMOVEPROCESSOR_H
#define PLUGINMOVEPROCESSOR_H

#include <QtGui/QWidget>
#include <QtCore/QVariantAnimation>
#include <QtCore/QEvent>
#include "plugin.h"

class RazorPanelLayout;
class QLayoutItem;


class PluginMoveProcessor : public QWidget
{
    Q_OBJECT
public:
    explicit PluginMoveProcessor(RazorPanelLayout *layout, Plugin *plugin);
    ~PluginMoveProcessor();

    Plugin *plugin() const { return mPlugin; }

signals:
    void finished();

public slots:
    void start();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void doStart();
    void doFinish();

private:
    enum MarkType
    {
        TopMark,
        BottomMark,
        LeftMark,
        RightMark
    };

    struct MousePosInfo
    {
        int index;
        QLayoutItem *item;
        bool after;
    };

    RazorPanelLayout *mLayout;
    Plugin *mPlugin;
    int mDestIndex;

    MousePosInfo itemByMousePos(const QPoint mouse) const;
    void drawMark(QLayoutItem *item, MarkType markType);
};


class CursorAnimation: public QVariantAnimation
{
    Q_OBJECT
public:
    void updateCurrentValue(const QVariant &value) { QCursor::setPos(value.toPoint()); }
};

#endif // PLUGINMOVEPROCESSOR_H
