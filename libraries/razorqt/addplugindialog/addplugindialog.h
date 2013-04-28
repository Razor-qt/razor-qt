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


#ifndef ADDPLUGINDIALOG_H
#define ADDPLUGINDIALOG_H

#include <QDialog>
#include <razorqt/razorplugininfo.h>

#define SEARCH_DELAY 125


namespace Ui {
    class AddPluginDialog;
}

/*! The AddPluginDialog class provides a dialog that allow users to add plugins.
 */
class AddPluginDialog : public QDialog
{
    Q_OBJECT

public:
    /*! Constructs a dialog with the given parent that initially displays
       RazorPluginInfo objects for the matched files in the directories
      @param desktopFilesDirs - list of the scanned directories names.
      @param serviceType - type of the plugin, for example "RazorPanel/Plugin".
      @param nameFilter  - wildcard filter that understands * and ? wildcards. */
    AddPluginDialog(const QStringList& desktopFilesDirs,
                    const QString& serviceType,
                    const QString& nameFilter="*",
                    QWidget *parent = 0);

    ~AddPluginDialog();

    void setPluginsInUse(const RazorPluginInfoList pluginsInUse);

signals:
    void pluginSelected(const RazorPluginInfo &plugin);

protected:
    void timerEvent(QTimerEvent* event);

private:
    void init();
    Ui::AddPluginDialog *ui;
    RazorPluginInfoList mPlugins;
    RazorPluginInfoList mPluginsInUse;
    int mTimerId;

private slots:
    void emitPluginSelected();
    void searchEditTexChanged(const QString& text);
    void toggleAddButtonState();
};

#endif // ADDPLUGINDIALOG_H
