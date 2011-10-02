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


#include "wmselectdialog.h"
#include "ui_wmselectdialog.h"
#include <QtGui/QListView>
#include <QtCore/QVariant>
#include <stdlib.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>


WmSelectDialog::WmSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WmSelectDialog)
{
    ui->setupUi(this);
    connect(ui->wmList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
}

WmSelectDialog::~WmSelectDialog()
{
    delete ui;
}


QString WmSelectDialog::windowManager() const
{
    QListWidgetItem *item = ui->wmList->currentItem();
    if (item)
        return item->data(Qt::UserRole).toString();

    return "";
}

bool findProgram(const QString &program)
{
    QString path = getenv("PATH");
    foreach(QString dir, path.split(":"))
    {

        QFileInfo fi= QFileInfo(dir + QDir::separator() + program);
        if (fi.isExecutable() )
            return true;
    }
    return false;
}

void WmSelectDialog::addWindowManager(const QString &program, const QString &description)
{
     QListWidgetItem *item = new QListWidgetItem(description, ui->wmList);
     item->setData(Qt::UserRole, QVariant(program));
     bool enable = findProgram(program);
     if (!enable)
        item->setFlags(Qt::NoItemFlags);

     if (enable && ui->wmList->currentRow() < 0)
     {
        ui->wmList->setCurrentItem(item);
     }
}
