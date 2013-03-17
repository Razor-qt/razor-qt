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


#ifndef RAZORPANEL_H
#define RAZORPANEL_H

#include <QtGui/QFrame>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include "irazorpanel.h"

class QMenu;
class Plugin;
class RazorSettings;
class RazorPluginInfo;
class RazorPanelLayout;

/*! \brief The RazorPanel class provides a single razor-panel.
 */
class RazorPanel : public QFrame, public IRazorPanel
{
    Q_OBJECT

    Q_PROPERTY(QString position READ qssPosition)

public:
    enum Alignment {
        AlignmentLeft   = -1,
        AlignmentCenter =  0,
        AlignmentRight  =  1
    };

    RazorPanel(const QString &configFile, QWidget *parent = 0);
    virtual ~RazorPanel();

    void readSettings();

    void showPopupMenu(Plugin *plugin = 0);
    void x11EventFilter(XEvent* event);

    // IRazorPanel .........................
    IRazorPanel::Position position() const { return mPosition; }
    QRect globalGometry() const;
    QRect calculatePopupWindowPos(const IRazorPanelPlugin *plugin, const QSize &windowSize) const;

    // For QSS properties ..................
    QString qssPosition() const;

    static bool canPlacedOn(int screenNum, RazorPanel::Position position);
    static QString positionToStr(IRazorPanel::Position position);
    static IRazorPanel::Position strToPosition(const QString &str, IRazorPanel::Position defaultValue);


    // Settings
    int lineSize() const { return mLineSize; }
    int lineCount() const { return mLineCount; }
    int length() const { return mLength; }
    bool lengthInPercents() const { return mLengthInPercents; }
    RazorPanel::Alignment alignment() const { return mAlignment; }
    int screenNum() const { return mScreenNum; }

    RazorSettings *settings() const { return mSettings; }

public slots:
    void show();

    // Settings
    void setLineSize(int value);
    void setLineCount(int value);
    void setLength(int length, bool inPercents);
    void setPosition(int screen, IRazorPanel::Position position);
    void setAlignment(RazorPanel::Alignment value);

    void saveSettings(bool later=false);

signals:
    void realigned();
    
protected:
    bool event(QEvent *event);

private slots:
    void screensChangeds();
    void addPlugin(const RazorPluginInfo &desktopFile);
    void showConfigDialog();
    void showAddPluginDialog();
    void realign();
    void removePlugin();
    void pluginMoved();

private:
    RazorPanelLayout* mLayout;
    RazorSettings *mSettings;
    QList<Plugin*> mPlugins;

    int findAvailableScreen(RazorPanel::Position position);

    void loadPlugins();
    Plugin *loadPlugin(const RazorPluginInfo &desktopFile, const QString &settingsGroup);
    Plugin *findPlugin(const IRazorPanelPlugin *iPlugin) const;

    QString findNewPluginSettingsGroup(const QString &pluginType) const;

    int mLineSize;
    int mLineCount;

    int mLength;
    bool mLengthInPercents;

    RazorPanel::Alignment mAlignment;

    IRazorPanel::Position mPosition;
    int mScreenNum;
    QTimer mDelaySave;
};


#endif // RAZORPANEL_H
