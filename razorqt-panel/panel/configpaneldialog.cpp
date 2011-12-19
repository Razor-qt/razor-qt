/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion-self@mail.ru>
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

//#include <QtGui/QMessageBox>
#include "configpaneldialog.h"
#include "ui_configpaneldialog.h"
//#include "razorpanel_p.h"
#include "razorpanel.h"
//#include <QtCore/QtDebug>


#define CFG_PANEL_GROUP     "panel"
#define CFG_KEY_HEIGHT      "height"

ConfigPanelDialog::ConfigPanelDialog(int hDefault, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPanelDialog)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(defaultClicked()));
    connect(ui->spinBox, SIGNAL(valueChanged(int)),this, SLOT(spinBoxValueChanged(int)));
    heightDefault=hDefault;

    // FIXME
    // I think, need use something like this
    // int mHeight = settings().value(CFG_KEY_HEIGHT, 37).toInt();

    mConfigFile = "panel";
    if (qApp->arguments().count() > 1)
    {
        mConfigFile = qApp->arguments().at(1);
        if (mConfigFile.endsWith(".conf"))
            mConfigFile.chop(5);
    }

    mSettings = new RazorSettings("razor-panel/" + mConfigFile, this);
    mSettings->beginGroup(CFG_PANEL_GROUP);
    int mHeight = mSettings->value(CFG_KEY_HEIGHT, heightDefault).toInt();
    mSettings->endGroup();

    ui->spinBox->setValue(mHeight);
}

ConfigPanelDialog::~ConfigPanelDialog()
{
    delete ui;
}

void ConfigPanelDialog::saveSettings()
{
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_HEIGHT, ui->spinBox->value());
    mSettings->endGroup();

    RazorPanel *parent = qobject_cast<RazorPanel*>(this->parentWidget());
    parent->show();
    ///RazorPanelPrivate *aaa = new RazorPanelPrivate(this);
    //aaa->realign();
    //emit realign();
    //qDebug() << "** Click " << "*************";

    this->close();
}

void ConfigPanelDialog::defaultClicked()
{
    ui->spinBox->setValue(heightDefault);
}

void ConfigPanelDialog::spinBoxValueChanged(int q)
{
    RazorPanel *parent = qobject_cast<RazorPanel*>(this->parentWidget());
    parent->show();
}
