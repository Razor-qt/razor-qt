/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#ifndef RAZORPANELPLUGIN_P_H
#define RAZORPANELPLUGIN_P_H

#include "razorpanelplugin.h"
#include "razorqt/razorsettings.h"
#include <QtCore/QObject>
#include <QtCore/QSettings>

class QBoxLayout;


class RazorPanelPluginPrivate: public QObject
{
    Q_OBJECT
public:
    RazorPanelPluginPrivate(const RazorPanelPluginStartInfo* startInfo, RazorPanelPlugin* parent);
    virtual ~RazorPanelPluginPrivate();

    QSettings& settings() const { return *mSettings; }

    RazorPanelPlugin::Alignment alignment() const;
    void setAlignment(RazorPanelPlugin::Alignment alignment);

    QString configId() const { return mConfigId; }

    void layoutDirectionChanged(QBoxLayout::Direction direction);

    void addWidget(QWidget* widget);

    bool isMovable() const { return mMovable; }
    void setMovable(bool movable);

    void updateStyleSheet();

    void paintEvent(QPaintEvent* event);

    RazorPanel* panel() const { return mPanel; }

    void contextMenuEvent(QContextMenuEvent* event);

    QMenu* popupMenu() const;

private:
    RazorPanelPlugin* const q_ptr;
    Q_DECLARE_PUBLIC(RazorPanelPlugin)

    RazorSettings* const mSettings;
    QString mConfigId;
    mutable RazorPanelPlugin::Alignment mAlignment;
    mutable bool mAlignmentCached;

    bool mMovable;
    RazorPanel* mPanel;

    QRect handleRect();
    void initStyleOption(QStyleOptionToolBar *option) const;

};

#endif // RAZORPANELPLUGIN_P_H
