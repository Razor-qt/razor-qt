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



#ifndef RAZORPANEL_P_H
#define RAZORPANEL_P_H

#include "razorpanel.h"
#include "razorpanelplugin.h"
#include <razorqt/razorplugininfo.h>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/QAction>
#include <QtCore/QVariantAnimation>

class QActionGroup;
class QLayoutItem;
class RazorSettings;
class RazorPanelLayout;
class AddPluginDialog;

class RazorPanelPrivate: QObject {
    Q_OBJECT
public:
    RazorPanelPrivate(RazorPanel* parent);
    virtual ~RazorPanelPrivate();

    void init();

    RazorPanel::Position position() const { return mPosition; }

    QList<RazorPanelPlugin*>& plugins() { return mPlugins; }
    bool canPlacedOn(int screenNum, RazorPanel::Position position) const;

    QMenu* popupMenu(QWidget *parent) const;

    void realign();

    virtual void contextMenuEvent( QContextMenuEvent* event);
    void saveSettings();

public slots:
    void switchPosition();
    void pluginMoved(QWidget* pluginWidget);
    void screensChangeds();
    void showAddPluginDialog();
    void showConfigPanelDialog();
    void addPlugin(const RazorPluginInfo &pluginInfo);
    void onRemovePlugin();
    void onMovePlugin();
    void startMoveWidget();
    void updateSize(int height, int width, bool percent, RazorPanel::Alignment alignment);

private:
    void loadPlugins();
    RazorPanelPlugin* loadPlugin(const RazorPluginInfo* pluginInfo, const QString configSection);
    void reTheme();
    int findAvailableScreen(RazorPanel::Position position);

    RazorPanel* const q_ptr;
    Q_DECLARE_PUBLIC(RazorPanel)

    RazorPanel::Position mPosition;
    RazorPanel::Alignment mAlignment;
    int mHeight;
    int mWidth;
    bool mWidthInPercents;
    int mScreenNum;
    QString mConfigFile;
    RazorSettings* mSettings;
    QList<RazorPanelPlugin*> mPlugins;
    RazorPanelLayout* mLayout;
    QLayoutItem* mSpacer;
};


class PositionAction: public QAction
{
  Q_OBJECT
public:
    PositionAction(int displayNum, RazorPanel::Position position, QActionGroup* parent = 0);

    RazorPanel::Position position() const { return mPosition; }
    int displayNum() const { return mDisplayNum; }

private:
    RazorPanel::Position mPosition;
    int mDisplayNum;
};


class PluginAction: public QAction
{
  Q_OBJECT
public:
    PluginAction(RazorPanelPlugin* plugin, const QString &text, QObject* parent):
        QAction(text, parent), mPlugin(plugin) {}

    PluginAction(RazorPanelPlugin* plugin, const QIcon &icon, const QString &text, QObject* parent):
        QAction(icon, text, parent), mPlugin(plugin) {}

    RazorPanelPlugin* plugin() const { return mPlugin; }
private:
    RazorPanelPlugin* mPlugin;
};

class CursorAnimation: public QVariantAnimation
{
    Q_OBJECT

public:
    CursorAnimation(QObject *parent);

    void updateCurrentValue(const QVariant &value);
};

#endif // RAZORPANEL_P_H
