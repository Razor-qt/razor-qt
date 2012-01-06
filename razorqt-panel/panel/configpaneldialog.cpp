/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
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

#define CFG_PANEL_GROUP     "panel"
#define CFG_KEY_HEIGHT      "height"
#define CFG_KEY_WIDTH       "width"
#define CFG_KEY_PERCENT     "width-percent"
#define CFG_KEY_ALIGNMENT   "alignment"


ConfigPanelDialog::ConfigPanelDialog(int hDefault, int wMax, RazorSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPanelDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->spinBox_height, SIGNAL(valueChanged(int)),this, SLOT(spinBoxHeightValueChanged(int)));
    connect(ui->comboBox_widthType, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxWidthTypeIndexChanged(int)));
    connect(ui->comboBox_alignment, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAlignmentIndexChanged(int)));
    connect(ui->spinBox_width, SIGNAL(valueChanged(int)),this, SLOT(spinBoxWidthValueChanged(int)));
    mHeightDefault=hDefault;
    mWidthMax=wMax;
    mSettings = settings;
    mCache = new RazorSettingsCache(mSettings);

    initControls();
}

void ConfigPanelDialog::initControls()
{
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mHeight = mSettings->value(CFG_KEY_HEIGHT, mHeightDefault).toInt();
    mWidthInPercents = mSettings->value(CFG_KEY_PERCENT, true).toBool();
    mWidth = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
    mAlignment = RazorPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, RazorPanel::AlignmentCenter).toInt());
    mSettings->endGroup();

    ui->spinBox_height->setValue(mHeight);
    ui->spinBox_width->setMaximum(mWidthInPercents ? 100 : mWidthMax);
    ui->spinBox_width->setValue(mWidth);
    ui->comboBox_widthType->setCurrentIndex(mWidthInPercents ? 0 : 1);
    ui->comboBox_alignment->setCurrentIndex(mAlignment + 1);
    emit configChanged(mHeight, mWidth, mWidthInPercents, mAlignment);
}

ConfigPanelDialog::~ConfigPanelDialog()
{
    delete ui;
    delete mCache;
}

void ConfigPanelDialog::dialogButtonsAction(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole)
    {
        mCache->loadToSettings();
        initControls();
    }
    else
    {
        close();
    }
}

void ConfigPanelDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_WIDTH, mWidth);
    mSettings->setValue(CFG_KEY_PERCENT, mWidthInPercents);
    mSettings->setValue(CFG_KEY_HEIGHT, mHeight);
    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);
    mSettings->endGroup();
    mSettings->sync();
}

void ConfigPanelDialog::spinBoxWidthValueChanged(int q)
{
    mWidth=q;
    // if panel width not max, user can plased it on left/rigth/center
    if ((mWidthInPercents && mWidth < 100) || (!mWidthInPercents && mWidth < mWidthMax))
        ui->comboBox_alignment->setEnabled(true);
    else
       ui->comboBox_alignment->setEnabled(false);

    emit configChanged(mHeight, mWidth, mWidthInPercents, mAlignment);
}

void ConfigPanelDialog::comboBoxWidthTypeIndexChanged(int q)
{
    bool inPercents = (q == 0);
    if (inPercents == mWidthInPercents)
        return;
    mWidthInPercents = inPercents;

    int width;
    if (mWidthInPercents)  // %
        width = mWidth * 100 / mWidthMax;
    else                // px
        width = (mWidth * mWidthMax) / 100;

    ui->spinBox_width->setMaximum(mWidthInPercents ? 100 : mWidthMax);
    ui->spinBox_width->setValue(width);
    mWidth = width;
}

void ConfigPanelDialog::comboBoxAlignmentIndexChanged(int q)
{
    mAlignment = RazorPanel::Alignment(q - 1);
    emit configChanged(mHeight, mWidth, mWidthInPercents, mAlignment);
}

void ConfigPanelDialog::spinBoxHeightValueChanged(int q)
{
    mHeight=q;
    emit configChanged(mHeight, mWidth, mWidthInPercents, mAlignment);
}
