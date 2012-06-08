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
#include "razorpanel_p.h"

#include <QtGui/QDesktopWidget>

struct ScreenPosition
{
    int screen;
    RazorPanel::Position position;
};
Q_DECLARE_METATYPE(ScreenPosition)

ConfigPanelDialog::ConfigPanelDialog(int hDefault, int wMax, RazorSettings *settings, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanelDialog),
    mSettings(settings)
{
    ui->setupUi(this);
    mSizeDefault = hDefault;
    mLengthMax = wMax;
    reset();

    connect(ui->spinBox_size, SIGNAL(valueChanged(int)),this, SLOT(spinBoxHeightValueChanged(int)));
    connect(ui->comboBox_widthType, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxWidthTypeIndexChanged(int)));
    connect(ui->comboBox_alignment, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxAlignmentIndexChanged(int)));
    connect(ui->comboBox_position, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxPositionIndexChanged(int)));
    connect(ui->spinBox_length, SIGNAL(valueChanged(int)),this, SLOT(spinBoxWidthValueChanged(int)));
    connect(ui->checkBox_useTheme, SIGNAL(toggled(bool)), this, SLOT(checkBoxUseThemeSizeChanged(bool)));
}

void ConfigPanelDialog::reset()
{
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSize = mSettings->value(CFG_KEY_HEIGHT, mSizeDefault).toInt();
    mWidthInPercents = mSettings->value(CFG_KEY_PERCENT, true).toBool();
    mLength = mSettings->value(CFG_KEY_WIDTH, 100).toInt();
    mUseThemeSize = mSettings->value(CFG_KEY_THEMESIZE, true).toBool();
    mAlignment = RazorPanel::Alignment(mSettings->value(CFG_KEY_ALIGNMENT, RazorPanel::AlignmentCenter).toInt());
    mPosition = RazorPanelPrivate::strToPosition(mSettings->value(CFG_KEY_POSITION).toString(), RazorPanel::PositionBottom);
    mScreenNum = mSettings->value(CFG_KEY_SCREENNUM, QApplication::desktop()->primaryScreen()).toInt();
    mSettings->endGroup();

    ui->spinBox_size->setValue(mSize);
    ui->spinBox_length->setMaximum(mWidthInPercents ? 100 : mLengthMax);
    ui->spinBox_length->setValue(mLength);
    ui->comboBox_widthType->setCurrentIndex(mWidthInPercents ? 0 : 1);
    ui->comboBox_alignment->setCurrentIndex(mAlignment + 1);
    ui->checkBox_useTheme->setChecked(mUseThemeSize);

    if (ui->comboBox_position->count() == 0)
    {
        int screenCount = QApplication::desktop()->screenCount();
        if (screenCount == 1)
        {
            addPosition(tr("Top of desktop"), 0, RazorPanel::PositionTop);
            addPosition(tr("Left of desktop"), 0, RazorPanel::PositionLeft);
            addPosition(tr("Right of desktop"), 0, RazorPanel::PositionRight);
            addPosition(tr("Bottom of desktop"), 0, RazorPanel::PositionBottom);
        }
        else
        {
            for (int screenNum = 0; screenNum < screenCount; screenNum++)
            {
                addPosition(tr("Top of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionTop);
                addPosition(tr("Left of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionLeft);
                addPosition(tr("Right of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionRight);
                addPosition(tr("Bottom of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionBottom);
            }
        }
    }

    for (int ix = 0; ix < ui->comboBox_position->count(); ix++)
    {
        ScreenPosition sp = ui->comboBox_position->itemData(ix).value<ScreenPosition>();
        if (mScreenNum == sp.screen && mPosition == sp.position)
            ui->comboBox_position->setCurrentIndex(ix);
    }

    // checkBoxUseThemeSizeChanged emits the configChanged signal
    // if mUseThemeSize is true it disables the size spinbox
    checkBoxUseThemeSizeChanged(mUseThemeSize);
    emit positionChanged(mScreenNum, mPosition);
}

ConfigPanelDialog::~ConfigPanelDialog()
{
    delete ui;
}

void ConfigPanelDialog::save()
{
    // Save is separate here to reduce disk I/O while resizing
    mSettings->beginGroup(CFG_PANEL_GROUP);
    mSettings->setValue(CFG_KEY_WIDTH, mLength);
    mSettings->setValue(CFG_KEY_PERCENT, mWidthInPercents);
    mSettings->setValue(CFG_KEY_HEIGHT, mSize);
    mSettings->setValue(CFG_KEY_ALIGNMENT, mAlignment);
    mSettings->setValue(CFG_KEY_THEMESIZE, mUseThemeSize);
    mSettings->setValue(CFG_KEY_POSITION, RazorPanelPrivate::positionToStr(mPosition));
    mSettings->setValue(CFG_KEY_SCREENNUM, mScreenNum);
    mSettings->endGroup();
}

void ConfigPanelDialog::addPosition(const QString& name, int screen, RazorPanel::Position position)
{
    if (RazorPanelPrivate::canPlacedOn(screen, position))
        ui->comboBox_position->addItem(name, QVariant::fromValue((ScreenPosition){screen, position}));
}

void ConfigPanelDialog::spinBoxWidthValueChanged(int q)
{
    mLength=q;
    // if panel width not max, user can plased it on left/rigth/center
    if ((mWidthInPercents && mLength < 100) || (!mWidthInPercents && mLength < mLengthMax))
        ui->comboBox_alignment->setEnabled(true);
    else
       ui->comboBox_alignment->setEnabled(false);

    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, mUseThemeSize);
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
    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, mUseThemeSize);
}

void ConfigPanelDialog::comboBoxPositionIndexChanged(int q)
{
    ScreenPosition sp = ui->comboBox_position->itemData(q).value<ScreenPosition>();
    mPosition = sp.position;
    mScreenNum = sp.screen;
    emit positionChanged(mScreenNum, mPosition);
}

void ConfigPanelDialog::spinBoxHeightValueChanged(int q)
{
    mSize=q;
    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, mUseThemeSize);
}

void ConfigPanelDialog::checkBoxUseThemeSizeChanged(bool state)
{
    mUseThemeSize = state;

    if (mUseThemeSize)
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

    emit configChanged(mSize, mLength, mWidthInPercents, mAlignment, mUseThemeSize);
}
