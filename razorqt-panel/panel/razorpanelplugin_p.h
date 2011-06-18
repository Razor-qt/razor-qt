/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

    QBoxLayout* layout() const { return mLayout; }
    void layoutDirectionChanged(QBoxLayout::Direction direction);

    void addWidget(QWidget* widget);

    bool isMovable() const { return mMovable; }
    void setMovable(bool movable);

    void updateStyleSheet();

    void paintEvent(QPaintEvent* event);

    RazorPanel* panel() const { return mPanel; }

    void contextMenuEvent(QContextMenuEvent* event);

    QMenu* popupMenu(QWidget *parent);

private:
    RazorPanelPlugin* const q_ptr;
    Q_DECLARE_PUBLIC(RazorPanelPlugin)

    RazorSettings* const mSettings;
    QString mConfigId;
    mutable RazorPanelPlugin::Alignment mAlignment;
    mutable bool mAlignmentCached;

    QBoxLayout* mLayout;
    bool mMovable;
    RazorPanel* mPanel;

    QRect handleRect();
    void initStyleOption(QStyleOptionToolBar *option) const;

};

#endif // RAZORPANELPLUGIN_P_H
