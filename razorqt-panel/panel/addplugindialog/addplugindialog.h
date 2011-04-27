/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

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

#ifndef ADDPLUGINDIALOG_H
#define ADDPLUGINDIALOG_H

#include <QDialog>
#include <razorqt/razorplugininfo.h>

#define SEARCH_DELAY 125


namespace Ui {
    class AddPluginDialog;
}

class AddPluginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddPluginDialog(RazorPluginInfoList* plugins, QWidget *parent = 0);
    ~AddPluginDialog();

signals:
    void pluginSelected(RazorPluginInfo* plugin);

protected:
    void timerEvent(QTimerEvent* event);

private:
    Ui::AddPluginDialog *ui;
    RazorPluginInfoList* mPlugins;
    int mTimerId;

private slots:
    void emitPluginSelected();
    void searchEditTexChanged(const QString& text);
};

#endif // ADDPLUGINDIALOG_H
