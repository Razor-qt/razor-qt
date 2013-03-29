/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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

#include "main_window.hpp"
#include "actions.hpp"
#include "default_model.hpp"

#include <QItemSelectionModel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    m_actions = new Actions(this);
    m_defaultModel = new DefaultModel(m_actions, this);
    m_selectionModel = new QItemSelectionModel(m_defaultModel);

    actions_TV->setModel(m_defaultModel);

    actions_TV->setSelectionModel(m_selectionModel);

    connect(m_selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(selectionChanged(QItemSelection,QItemSelection)));
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::selectionChanged(const QItemSelection &/*selected*/, const QItemSelection &/*deselected*/)
{
    QModelIndexList rows = m_selectionModel->selectedRows();

    modify_PB->setEnabled(rows.length() == 1);
    changeShortcut_PB->setEnabled(rows.length() == 1);
    swap_PB->setEnabled(rows.length() == 2);
    remove_PB->setEnabled(rows.length() != 0);
}

void MainWindow::on_add_PB_clicked()
{
}

void MainWindow::on_modify_PB_clicked()
{
}

void MainWindow::on_changeShortcut_PB_clicked()
{
}

void MainWindow::on_swap_PB_clicked()
{
}

void MainWindow::on_remove_PB_clicked()
{
}
