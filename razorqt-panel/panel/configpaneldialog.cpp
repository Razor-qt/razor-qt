/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
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

#include "configpaneldialog.h"
#include "ui_configpaneldialog.h"

#define CFG_PANEL_GROUP     "panel"
#define CFG_KEY_HEIGHT      "height"
#define CFG_KEY_WIDTH       "width"
#define CFG_KEY_PERCENT     "width-percent"
#define CFG_KEY_ALIGNMENT   "alignment"
#define CFG_KEY_THEMESIZE   "theme-size"


ConfigPanelDialog::ConfigPanelDialog(int hDefault, int wMax, RazorSettings *settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigPanelDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->spinBox_size, SIGNAL(valueChanged(int)),this, SLOT(spinBoxHeightValueChanged(int)));
    connect(ui->comboBox_widthType, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxWidthTypeIndexChanged(int)));
    connect(ui->comboBox_alignment, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAlignmentIndexChanged(int)));
    connect(ui->spinBox_length, SIGNAL(valueChanged(int)),this, SLOT(spinBoxWidthValueChanged(int)));
    connect(ui->checkBox_useTheme, SIGNAL(toggled(bool)), this, SLOT(checkBoxUseThemeSizeChanged(bool)));
    mSizeDefault=hDefault;
    mLengthMax=wMax;
    mSettings = settings;
    mCache = new RazorSettingsCache(mSettings);

    initControls();
}

void ConfigPanelDialog::initControls()
{
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSize = mSettings->value(CFG_KEY_HEIGHT, mSizeDefault).toInt();
    mWidthInPercents = mSettings->value(CFG_KEY_PERCENT, true).toBool();
    mLength = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
    useThemeSize = mSettings->value(CFG_KEY_THEMESIZE, true).toBool();
    mAlignment = RazorPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, RazorPanel::AlignmentCenter).toInt());
    mSettings->endGroup();

    ui->spinBox_size->setValue(mSize);
    ui->spinBox_length->setMaximum(mWidthInPercents ? 100 : mLengthMax);
    ui->spinBox_length->setValue(mLength);
    ui->comboBox_widthType->setCurrentIndex(mWidthInPercents ? 0 : 1);
    ui->comboBox_alignment->setCurrentIndex(mAlignment + 1);
    ui->checkBox_useTheme->setChecked(useThemeSize);
    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, useThemeSize);
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
    mSettings->setValue(CFG_KEY_WIDTH, mLength);
    mSettings->setValue(CFG_KEY_PERCENT, mWidthInPercents);
    mSettings->setValue(CFG_KEY_HEIGHT, mSize);
    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);
    mSettings->setValue(CFG_KEY_THEMESIZE, useThemeSize);
    mSettings->endGroup();
    mSettings->sync();
}

void ConfigPanelDialog::spinBoxWidthValueChanged(int q)
{
    mLength=q;
    // if panel width not max, user can plased it on left/rigth/center
    if ((mWidthInPercents && mLength < 100) || (!mWidthInPercents && mLength < mLengthMax))
        ui->comboBox_alignment->setEnabled(true);
    else
       ui->comboBox_alignment->setEnabled(false);

    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, useThemeSize);
}

void ConfigPanelDialog::comboBoxWidthTypeIndexChanged(int q)
{
    bool inPercents = (q == 0);
    if (inPercents == mWidthInPercents)
        return;
    mWidthInPercents = inPercents;

    int width;
    if (mWidthInPercents)  // %
        width = mLength * 100 / mLengthMax;
    else                // px
        width = (mLength * mLengthMax) / 100;

    ui->spinBox_length->setMaximum(mWidthInPercents ? 100 : mLengthMax);
    ui->spinBox_length->setValue(width);
    mLength = width;
}

void ConfigPanelDialog::comboBoxAlignmentIndexChanged(int q)
{
    mAlignment = RazorPanel::Alignment(q - 1);
    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, useThemeSize);
}

void ConfigPanelDialog::spinBoxHeightValueChanged(int q)
{
    mSize=q;
    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, useThemeSize);
}

void ConfigPanelDialog::checkBoxUseThemeSizeChanged(bool state)
{
    useThemeSize = state;

    if (useThemeSize)
    {
        ui->label_size->setEnabled(false);
        ui->spinBox_size->setEnabled(false);
        ui->label_px->setEnabled(false);
    }
    else
    {
        ui->label_size->setEnabled(true);
        ui->spinBox_size->setEnabled(true);
        ui->label_px->setEnabled(true);
    }

    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, useThemeSize);
}
