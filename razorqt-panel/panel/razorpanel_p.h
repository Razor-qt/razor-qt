/********************************************************************
  Copyright: 2010-2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/


#ifndef RAZORPANEL_P_H
#define RAZORPANEL_P_H

#include "razorpanel.h"
#include "razorpanelplugin.h"
#include <razorqt/razorplugininfo.h>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtGui/QAction>

class QSettings;
class QActionGroup;
class QLayoutItem;
class ReadSettings;
class RazorPanelPluginInfo;
class RazorPanelLayout;


class RazorPanelPrivate: QObject {
    Q_OBJECT
public:
    RazorPanelPrivate(RazorPanel* parent);
    virtual ~RazorPanelPrivate();

    void init();

    RazorPanel::Position position() const { return mPosition; }

    QList<RazorPanelPlugin*>& plugins() { return mPlugins; }
    RazorPluginInfoList<RazorPanelPluginInfo>& availablePlugins() { return mAvailablePlugins; }
    bool canPlacedOn(int screenNum, RazorPanel::Position position) const;

    QMenu* popupMenu(QWidget *parent) const;

    void realign();

    virtual void contextMenuEvent( QContextMenuEvent* event);
    void saveSettings();

public slots:
    void switchPosition();
    void pluginMoved(QWidget* pluginWidget);
    void screensChangeds();

private:
    void loadPlugins();
    void setTheme(const QString& themeName);
    int findAvailableScreen(RazorPanel::Position position);

    RazorPanel* const q_ptr;
    Q_DECLARE_PUBLIC(RazorPanel)

    RazorPanel::Position mPosition;
    int mScreenNum;
    QString mConfigFile;
    QString mTheme;
    QSettings* mSettings;
    ReadSettings* mSettingsReader;
    RazorPluginInfoList<RazorPanelPluginInfo> mAvailablePlugins;
    QList<RazorPanelPlugin*> mPlugins;
    RazorPanelLayout* mLayout;
    QLayoutItem* mSpacer;
};


class RazorPanelPluginInfo: public RazorPluginInfo
{
public:
    RazorPanelPluginInfo(const QString& pluginDesktopFile, QObject *parent = 0): RazorPluginInfo(pluginDesktopFile, parent) {}
    RazorPanelPlugin* instance(const QString& configFile, const QString& configSection, QObject* parent = 0);

protected:
    QString translationDir() const { return TRANSLATIONS_DIR; }
    QString libraryDir() const { return PLUGIN_DIR; }

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


#endif // RAZORPANEL_P_H
