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


#ifndef PLUGIN_H
#define PLUGIN_H

#include <QFrame>
#include <QString>
#include <razorqt/razorplugininfo.h>

class QSettings;
class IRazorPanel;
class IRazorPanelPlugin;
class IRazorPanelPluginLibrary;
class RazorPanel;
class QMenu;

class Plugin : public QFrame
{
    Q_OBJECT
public:
    enum Alignment {
        AlignLeft,
        AlignRight
    };


    explicit Plugin(const RazorPluginInfo &desktopFile, const QString &settingsFile, const QString &settingsGroup, RazorPanel *panel);
    ~Plugin();

    bool isLoaded() const { return mPlugin != 0; }
    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment alignment) { mAlignment = alignment; }

    QString settingsGroup() const { return mSettingsGroup; }

    void saveSettings();
    void x11EventFilter(XEvent* event);

    QMenu* popupMenu() const;

signals:
    void move(QWidget *widget);
    void remove();

protected:
    virtual bool event(QEvent* e);

private:
    bool loadLib(const QString &libraryName);

    const RazorPluginInfo mDesktopFile;
    IRazorPanelPlugin *mPlugin;
    IRazorPanelPluginLibrary *mPluginLib;
    QWidget *mPluginWidget;
    Alignment mAlignment;
    QSettings *mSettings;
    QString mSettingsGroup;
    RazorPanel *mPanel;

private slots:
    void settingsChanged();
    void showConfigureDialog();
    void requestMove();
    void requestRemove();
};

#endif // PLUGIN_H
