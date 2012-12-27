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
#include "../razorpanel.h"

#include <QDebug>
#include <QtGui/QDesktopWidget>

#include <razorqt/xfitman.h>

struct ScreenPosition
{
    int screen;
    IRazorPanel::Position position;
};
Q_DECLARE_METATYPE(ScreenPosition)



/************************************************

 ************************************************/
ConfigPanelDialog *ConfigPanelDialog::exec(RazorPanel *panel)
{
    ConfigPanelDialog *dialog =
        panel->findChild<ConfigPanelDialog*>();

    if (!dialog)
    {
        dialog = new ConfigPanelDialog(panel, panel);
    }

    dialog->show();
    dialog->raise();
    dialog->activateWindow();
    xfitMan().raiseWindow(dialog->effectiveWinId());
    xfitMan().moveWindowToDesktop(dialog->effectiveWinId(), qMax(xfitMan().getActiveDesktop(), 0));
    return dialog;
}


/************************************************

 ************************************************/
ConfigPanelDialog::ConfigPanelDialog(RazorPanel *panel, QWidget *parent):
    RazorConfigDialog(tr("Configure panel"), panel->settings(), parent)
{
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);

    ConfigPanelWidget* page = new ConfigPanelWidget(panel, this);
    addPage(page, tr("Configure panel"));
    connect(this, SIGNAL(reset()), page, SLOT(reset()));
}


/************************************************

 ************************************************/
ConfigPanelWidget::ConfigPanelWidget(RazorPanel *panel, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigPanelWidget),
    mPanel(panel)
{
    ui->setupUi(this);
    fillComboBox_position();
    fillComboBox_alignment();

    mOldLineSize  = mPanel->lineSize();
    mOldLineCount = mPanel->lineCount();

    mOldLength = mPanel->length();
    mOldLengthInPercents = mPanel->lengthInPercents();

    mOldAlignment = mPanel->alignment();

    mOldScreenNum = mPanel->screenNum();
    mScreenNum = mOldScreenNum;

    mOldPosition = mPanel->position();
    mPosition = mOldPosition;

    reset();

    connect(ui->spinBox_lineSize,   SIGNAL(valueChanged(int)), this, SLOT(editChanged()));
    connect(ui->spinBox_lineCount,  SIGNAL(valueChanged(int)), this, SLOT(editChanged()));

    connect(ui->spinBox_length,     SIGNAL(valueChanged(int)), this, SLOT(editChanged()));
    connect(ui->comboBox_lenghtType,SIGNAL(activated(int)),    this, SLOT(widthTypeChanged()));

    connect(ui->comboBox_alignment, SIGNAL(activated(int)),    this, SLOT(editChanged()));
    connect(ui->comboBox_position,  SIGNAL(activated(int)),    this, SLOT(posittionChanged()));
}


/************************************************

 ************************************************/
void ConfigPanelWidget::reset()
{
    ui->spinBox_lineSize->setValue(mOldLineSize);
    ui->spinBox_lineCount->setValue(mOldLineCount);

    ui->comboBox_position->setCurrentIndex(indexForPosition(mOldScreenNum, mOldPosition));

    fillComboBox_alignment();
    ui->comboBox_alignment->setCurrentIndex(mOldAlignment + 1);

    ui->comboBox_lenghtType->setCurrentIndex(mOldLengthInPercents ? 0 : 1);
    widthTypeChanged();
    ui->spinBox_length->setValue(mOldLength);
}


/************************************************

 ************************************************/
void ConfigPanelWidget::fillComboBox_position()
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
            if (screenNum)
                ui->comboBox_position->insertSeparator(9999);

            addPosition(tr("Top of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionTop);
            addPosition(tr("Left of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionLeft);
            addPosition(tr("Right of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionRight);
            addPosition(tr("Bottom of desktop %1").arg(screenNum +1), screenNum, RazorPanel::PositionBottom);
        }
    }
}


/************************************************

 ************************************************/
void ConfigPanelWidget::fillComboBox_alignment()
{
    ui->comboBox_alignment->setItemData(0, QVariant(RazorPanel::AlignmentLeft));
    ui->comboBox_alignment->setItemData(1, QVariant(RazorPanel::AlignmentCenter));
    ui->comboBox_alignment->setItemData(2,  QVariant(RazorPanel::AlignmentRight));


    if (mPosition   == IRazorPanel::PositionTop ||
        mPosition   == IRazorPanel::PositionBottom)
    {
        ui->comboBox_alignment->setItemText(0, tr("Left"));
        ui->comboBox_alignment->setItemText(1, tr("Center"));
        ui->comboBox_alignment->setItemText(2, tr("Right"));
    }
    else
    {
        ui->comboBox_alignment->setItemText(0, tr("Top"));
        ui->comboBox_alignment->setItemText(1, tr("Center"));
        ui->comboBox_alignment->setItemText(2, tr("Bottom"));
    };
}


/************************************************

 ************************************************/
void ConfigPanelWidget::addPosition(const QString& name, int screen, RazorPanel::Position position)
{
    if (RazorPanel::canPlacedOn(screen, position))
        ui->comboBox_position->addItem(name, QVariant::fromValue((ScreenPosition){screen, position}));
}


/************************************************

 ************************************************/
int ConfigPanelWidget::indexForPosition(int screen, IRazorPanel::Position position)
{
    for (int i = 0; i < ui->comboBox_position->count(); i++)
    {
        ScreenPosition sp = ui->comboBox_position->itemData(i).value<ScreenPosition>();
        if (screen == sp.screen && position == sp.position)
            return i;
    }
    return -1;
}


/************************************************

 ************************************************/
ConfigPanelWidget::~ConfigPanelWidget()
{
    delete ui;
}


/************************************************

 ************************************************/
void ConfigPanelWidget::editChanged()
{
    mPanel->setLineSize(ui->spinBox_lineSize->value());
    mPanel->setLineCount(ui->spinBox_lineCount->value());

    mPanel->setLength(ui->spinBox_length->value(),
                      ui->comboBox_lenghtType->currentIndex() == 0);

    RazorPanel::Alignment align = RazorPanel::Alignment(
                ui->comboBox_alignment->itemData(
                    ui->comboBox_alignment->currentIndex()
                    ).toInt());

    mPanel->setAlignment(align);

    mPanel->setPosition(mScreenNum, mPosition);
}


/************************************************

 ************************************************/
void ConfigPanelWidget::widthTypeChanged()
{
    int max = getMaxLength();

    if (ui->comboBox_lenghtType->currentIndex() == 0)
    {
        // Percents .............................
        int v = ui->spinBox_length->value() * 100.0 / max;
        ui->spinBox_length->setMaximum(100);
        ui->spinBox_length->setValue(v);
        ui->spinBox_length->setSingleStep(1);
    }
    else
    {
        // Pixels ...............................
        int v =  max / 100.0 * ui->spinBox_length->value();
        ui->spinBox_length->setMaximum(max);
        ui->spinBox_length->setValue(v);
        ui->spinBox_length->setSingleStep(10);
    }
}


/************************************************

 ************************************************/
int ConfigPanelWidget::getMaxLength()
{
    QDesktopWidget* dw = QApplication::desktop();

    if (mPosition == IRazorPanel::PositionTop ||
        mPosition == IRazorPanel::PositionBottom)
    {
        return dw->screenGeometry(mScreenNum).width();
    }
    else
    {
        return dw->screenGeometry(mScreenNum).height();
    }
}


/************************************************

 ************************************************/
void ConfigPanelWidget::posittionChanged()
{
    ScreenPosition sp = ui->comboBox_position->itemData(
                ui->comboBox_position->currentIndex()).value<ScreenPosition>();

    bool updateAlig = (sp.position == IRazorPanel::PositionTop ||
                       sp.position == IRazorPanel::PositionBottom) !=
                      (mPosition   == IRazorPanel::PositionTop ||
                       mPosition   == IRazorPanel::PositionBottom);

    int oldMax = getMaxLength();
    mPosition = sp.position;
    mScreenNum = sp.screen;
    int newMax = getMaxLength();

    if (ui->comboBox_lenghtType->currentIndex() == 1 &&
        oldMax != newMax)
    {
        // Pixels ...............................
        int v = ui->spinBox_length->value() * 1.0 * newMax / oldMax;
        ui->spinBox_length->setMaximum(newMax);
        ui->spinBox_length->setValue(v);
    }

    if (updateAlig)
        fillComboBox_alignment();

    editChanged();
}
