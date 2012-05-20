/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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



#ifndef RAZORPANEL_P_H
#define RAZORPANEL_P_H

#include "razorpanel.h"
#include "razorpanelplugin.h"
#include <razorqt/razorplugininfo.h>
#include <QtGui/QAction>
#include <QtCore/QVariantAnimation>
#include <QtCore/QEvent>
#include <QtGui/QMenu>

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
    static bool canPlacedOn(int screenNum, RazorPanel::Position position);
    static RazorPanel::Position strToPosition(const QString &str, RazorPanel::Position defaultValue);
    static QString positionToStr(RazorPanel::Position position);

    void saveSettings();

public slots:
    void realign();
    void reposition();
    void switchPosition(int screenNum, RazorPanel::Position position);
    void pluginMoved(QWidget* pluginWidget);
    void screensChangeds();
    void showAddPluginDialog();
    void showConfigPanelDialog();
    void addPlugin(const RazorPluginInfo &pluginInfo);
    void onRemovePlugin();
    void onMovePlugin();
    void startMoveWidget();
    void updateSize(int height, int width, bool percent, RazorPanel::Alignment alignment, bool useThemeSize);

private:
    void loadPlugins();
    RazorPanelPlugin* loadPlugin(const RazorPluginInfo& pluginInfo, const QString configSection);
    void reTheme();
    int findAvailableScreen(RazorPanel::Position position);
    QStringList pluginDesktopDirs();

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
    bool mUseThemeSize;
    void updatePluginsMinSize();
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


class PopupMenu: public QMenu
{
public:
    explicit PopupMenu(QWidget *parent = 0): QMenu(parent) {}
    explicit PopupMenu(const QString &title, QWidget *parent = 0): QMenu(title, parent) {}

    QAction* addTitle(const QIcon &icon, const QString &text);
    QAction* addTitle(const QString &text);

    bool eventFilter(QObject *object, QEvent *event)
    {
        Q_UNUSED(object);

        if (event->type() == QEvent::Paint ||
            event->type() == QEvent::KeyPress ||
            event->type() == QEvent::KeyRelease
           )
        {
            return false;
        }

        event->accept();
        return true;
    }

protected:
    virtual void keyPressEvent(QKeyEvent* e);
};

#endif // RAZORPANEL_P_H
