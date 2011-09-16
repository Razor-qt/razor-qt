/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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


#include "razorpanelpluginconfigdialog.h"


#include <QtGui/QButtonGroup>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QComboBox>
#include <QDebug>
/************************************************

 ************************************************/
RazorPanelPluginConfigDialog::RazorPanelPluginConfigDialog(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    mSettings(settings),
    mOldSettings(settings)
{
}


/************************************************

 ************************************************/
RazorPanelPluginConfigDialog::~RazorPanelPluginConfigDialog()
{
}


/************************************************

 ************************************************/
QSettings& RazorPanelPluginConfigDialog::settings() const
{
    return mSettings;
}



/************************************************

 ************************************************/
void RazorPanelPluginConfigDialog::dialogButtonsAction(QAbstractButton *btn)
{
    QDialogButtonBox *box = qobject_cast<QDialogButtonBox*>(btn->parent());

    if (box && box->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}


/************************************************

 ************************************************/
void RazorPanelPluginConfigDialog::setComboboxIndexByData(QComboBox *comboBox, const QVariant &data, int defaultIndex) const
{
    int index = comboBox ->findData(data);
    if (index < 0)
        index = defaultIndex;

    comboBox->setCurrentIndex(index);
}
