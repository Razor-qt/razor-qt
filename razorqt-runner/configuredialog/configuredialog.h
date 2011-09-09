/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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


#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <QDialog>

namespace Ui {
    class ConfigureDialog;
}

class QSettings;
class RazorSettingsCache;

class ConfigureDialog : public QDialog
{
    Q_OBJECT

public:
    enum Position {
        PositionTop,
        PositionCenter
    };


    explicit ConfigureDialog(QSettings *settings, QWidget *parent = 0);
    ~ConfigureDialog();

    static ConfigureDialog *createAndShow(QSettings *settings, QWidget *parent);

protected:
   // void closeEvent(QCloseEvent *event);

private:
    Ui::ConfigureDialog *ui;
    QSettings *mSettings;
    RazorSettingsCache *mOldSettings;

private slots:
    void shortcutChanged(const QString &text);
    void settingsChanged();
    void positionCbxChanged(int index);
    void monitorCbxChanged(int index);
    void resetSettings();
};

#endif // CONFIGUREDIALOG_H
