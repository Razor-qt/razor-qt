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

#ifndef WMSELECTDIALOG_H
#define WMSELECTDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class WmSelectDialog;
}

class WmSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WmSelectDialog(QWidget *parent = 0);
    ~WmSelectDialog();
    QString windowManager() const;
    void addWindowManager(const QString &program, const QString &description);

private:
    Ui::WmSelectDialog *ui;

};

#endif // WMSELECTDIALOG_H
