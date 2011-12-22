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

#include "configpaneldialog.h"
#include "ui_configpaneldialog.h"
#include "razorpanel.h"

#define CFG_PANEL_GROUP     "panel"
#define CFG_KEY_HEIGHT      "height"
#define CFG_KEY_WIDTH       "width"
#define CFG_KEY_WIDTH_TYPE  "widthType"
#define CFG_KEY_ALIGNMENT   "alignment"


ConfigPanelDialog::ConfigPanelDialog(int hDefault, int wMax, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPanelDialog)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ui->spinBox_height, SIGNAL(valueChanged(int)),this, SLOT(spinBoxHeightValueChanged(int)));
    connect(ui->comboBox_widthType, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxWidthTypeIndexChanged(int)));
    connect(ui->comboBox_alignment, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAlignmentIndexChanged(int)));
    connect(ui->spinBox_width, SIGNAL(valueChanged(int)),this, SLOT(spinBoxWidthValueChanged(int)));
    mHeightDefault=hDefault;
    mWidthMax=wMax;

    // FIXME
    // I think, need use something like this
    // int mHeight = settings().value(CFG_KEY_HEIGHT, 37).toInt();
    // but I can't do in now =((

    mConfigFile = "panel";
    if (qApp->arguments().count() > 1)
    {
        mConfigFile = qApp->arguments().at(1);
        if (mConfigFile.endsWith(".conf"))
            mConfigFile.chop(5);
    }

    mSettings = new RazorSettings("razor-panel/" + mConfigFile, this);
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mHeight = mSettings->value(CFG_KEY_HEIGHT, mHeightDefault).toInt();
    mWidthType = mSettings->value(CFG_KEY_WIDTH_TYPE, 0).toInt();
    mAlignment = mSettings->value(CFG_KEY_ALIGNMENT, 2).toInt();
    if (mWidthType==0)      // size in percents
    {
        mWidth = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
        ui->spinBox_width->setMaximum(100);
    }
    else                    // size in pixels
    {
        ui->spinBox_width->setMaximum(mWidthMax);
        mWidth = mSettings->value(CFG_KEY_WIDTH, mWidthMax).toInt();
    }
    mSettings->endGroup();

    ui->spinBox_height->setValue(mHeight);
    ui->comboBox_widthType->setCurrentIndex(mWidthType);
    ui->spinBox_width->setValue(mWidth);
    ui->comboBox_alignment->setCurrentIndex(mAlignment);
}

ConfigPanelDialog::~ConfigPanelDialog()
{
    delete ui;
}

void ConfigPanelDialog::saveSettings()
{
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_WIDTH, mWidth);
    mSettings->setValue(CFG_KEY_WIDTH_TYPE, mWidthType);
    mSettings->setValue(CFG_KEY_HEIGHT, mHeight);
    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);
    mSettings->endGroup();

    RazorPanel *parent = qobject_cast<RazorPanel*>(this->parentWidget());
    parent->show();
    this->close();
}

void ConfigPanelDialog::spinBoxWidthValueChanged(int q)
{
    mWidth=q;
    // if panel width not max, user can plased it on left/rigth/center
    if ((mWidthType==0 && ui->spinBox_width->value()<100) || (mWidthType==1 && ui->spinBox_width->value()<mWidthMax))
        ui->comboBox_alignment->setEnabled(true);
    else
       ui->comboBox_alignment->setEnabled(false);
}

void ConfigPanelDialog::comboBoxWidthTypeIndexChanged(int q)
{
    mWidthType=q;
    if (mWidthType==0)  // %
        //ui->spinBox_width->setValue(mWidth*100/mWidthMax);        // px to %
        ui->spinBox_width->setMaximum(100);
    else                // px
        //ui->spinBox_width->setValue((mWidth*mWidthMax)/100);        // % to px
        ui->spinBox_width->setMaximum(mWidthMax);
}

void ConfigPanelDialog::comboBoxAlignmentIndexChanged(int q)
{
    mAlignment=q;
}

void ConfigPanelDialog::spinBoxHeightValueChanged(int q)
{
    mHeight=q;
}
