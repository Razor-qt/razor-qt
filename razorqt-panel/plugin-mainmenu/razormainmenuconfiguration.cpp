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

#include "razormainmenuconfiguration.h"
#include "ui_razormainmenuconfiguration.h"
#include "razorqt/xdgmenu.h"

#include <QtGui/QFileDialog>

RazorMainMenuConfiguration::RazorMainMenuConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorMainMenuConfiguration),
    mSettings(settings),
    mOldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("MainMenuConfigurationWindow");
    ui->setupUi(this);

    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->showTextCB, SIGNAL(toggled(bool)), ui->textL, SLOT(setEnabled(bool)));
    connect(ui->showTextCB, SIGNAL(toggled(bool)), ui->textLE, SLOT(setEnabled(bool)));

    loadSettings();

    connect(ui->showTextCB, SIGNAL(toggled(bool)), this, SLOT(showTextChanged(bool)));
    connect(ui->textLE, SIGNAL(textEdited(QString)), this, SLOT(textButtonChanged(QString)));
    connect(ui->chooseMenuFilePB, SIGNAL(clicked()), this, SLOT(chooseMenuFile()));
}

RazorMainMenuConfiguration::~RazorMainMenuConfiguration()
{
    delete ui;
}

void RazorMainMenuConfiguration::loadSettings()
{
    ui->showTextCB->setChecked(mSettings.value("showText", false).toBool());
    ui->textLE->setText(mSettings.value("text", "").toString());
    ui->menuFilePathLE->setText(mSettings.value("menu_file", XdgMenu::getMenuFileName()).toString());
}

void RazorMainMenuConfiguration::textButtonChanged(QString value)
{
    mSettings.setValue("text", value);
}

void RazorMainMenuConfiguration::showTextChanged(bool value)
{
    mSettings.setValue("showText", value);
}

void RazorMainMenuConfiguration::chooseMenuFile()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Choose menu file"), "~", tr("Menu files (*.menu)"));
    if (!path.isEmpty())
    {
        ui->menuFilePathLE->setText(path);
        mSettings.setValue("menu_file", path);
    }
}

void RazorMainMenuConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}
