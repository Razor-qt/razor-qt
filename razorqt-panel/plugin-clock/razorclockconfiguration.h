/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2011 Razor team
 * Authors:
 *   Maciej Płaza <plaza.maciej@gmail.com>
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

#ifndef RAZORCLOCKCONFIGURATION_H
#define RAZORCLOCKCONFIGURATION_H

#include <QtGui/QDialog>
#include <QtGui/QAbstractButton>
#include <QtGui/QButtonGroup>
#include <QtCore/QSettings>
#include <QtCore/QLocale>
#include <QtCore/QDateTime>

namespace Ui {
    class RazorClockConfiguration;
}

class RazorClockConfiguration : public QDialog
{
    Q_OBJECT

public:
    explicit RazorClockConfiguration(QSettings &settings, QWidget *parent = 0);
    ~RazorClockConfiguration();

private:
    Ui::RazorClockConfiguration *ui;
    QSettings &mSettings;
    QHash<QString, QVariant> oldSettings;

    /*
      Read settings from conf file and put data into controls.
    */
    void loadSettings();

    /*
      Creates a date formats consistent with the region read from locale.
    */
    void createDateFormats();

private slots:
    /*
      Saves settings in conf file.
    */
    void saveSettings();
    void rejectChanges();
};

#endif // RAZORCLOCKCONFIGURATION_H
