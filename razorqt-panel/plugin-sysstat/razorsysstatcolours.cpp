/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
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


#include "razorsysstatcolours.h"
#include "ui_razorsysstatcolours.h"

#include <QtCore/QSignalMapper>
#include <QtGui/QColorDialog>


RazorSysStatColours::RazorSysStatColours(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorSysStatColours),
    mSelectColourMapper(new QSignalMapper(this))
{
    ui->setupUi(this);

    mDefaultColours["grid"]  = QColor("#808080");
    mDefaultColours["title"] = QColor("#000000");

    mDefaultColours["cpuSystem"]    = QColor("#800000");
    mDefaultColours["cpuUser"]      = QColor("#000080");
    mDefaultColours["cpuNice"]      = QColor("#008000");
    mDefaultColours["cpuOther"]     = QColor("#808000");
    mDefaultColours["cpuFrequency"] = QColor("#808080");

    mDefaultColours["memApps"]    = QColor("#000080");
    mDefaultColours["memBuffers"] = QColor("#008000");
    mDefaultColours["memCached"]  = QColor("#808000");
    mDefaultColours["memSwap"]    = QColor("#800000");

    mDefaultColours["netReceived"]    = QColor("#000080");
    mDefaultColours["netTransmitted"] = QColor("#808000");


#undef CONNECT_SELECT_COLOUR
#define CONNECT_SELECT_COLOUR(VAR) \
    connect(ui-> VAR ## B, SIGNAL(clicked()), mSelectColourMapper, SLOT(map())); \
    mSelectColourMapper->setMapping(ui-> VAR ## B, QString( #VAR )); \
    mShowColourMap[QString( #VAR )] = ui-> VAR ## B;

    CONNECT_SELECT_COLOUR(grid)
    CONNECT_SELECT_COLOUR(title)
    CONNECT_SELECT_COLOUR(cpuSystem)
    CONNECT_SELECT_COLOUR(cpuUser)
    CONNECT_SELECT_COLOUR(cpuNice)
    CONNECT_SELECT_COLOUR(cpuOther)
    CONNECT_SELECT_COLOUR(cpuFrequency)
    CONNECT_SELECT_COLOUR(memApps)
    CONNECT_SELECT_COLOUR(memBuffers)
    CONNECT_SELECT_COLOUR(memCached)
    CONNECT_SELECT_COLOUR(memSwap)
    CONNECT_SELECT_COLOUR(netReceived)
    CONNECT_SELECT_COLOUR(netTransmitted)

#undef CONNECT_SELECT_COLOUR

    connect(mSelectColourMapper, SIGNAL(mapped(const QString &)), SLOT(selectColour(const QString &)));
}

RazorSysStatColours::~RazorSysStatColours()
{
    delete ui;
}

void RazorSysStatColours::selectColour(const QString &name)
{
    QColor color = QColorDialog::getColor(mColours[name], this);
    if (color.isValid())
    {
        mColours[name] = color;
        mShowColourMap[name]->setStyleSheet(QString("background-color: %1;\ncolor: %2;").arg(color.name()).arg((color.toHsl().lightnessF() > 0.5) ? "black" : "white"));

        ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(true);
    }
}

void RazorSysStatColours::setColours(const Colours &colours)
{
    mInitialColours = colours;
    mColours = colours;
    applyColoursToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
}

void RazorSysStatColours::applyColoursToButtons()
{
    Colours::ConstIterator M = mColours.constEnd();
    for (Colours::ConstIterator I = mColours.constBegin(); I != M; ++I)
    {
        const QColor &color = I.value();
        mShowColourMap[I.key()]->setStyleSheet(QString("background-color: %1;\ncolor: %2;").arg(color.name()).arg((color.toHsl().lightnessF() > 0.5) ? "black" : "white"));
    }
}

void RazorSysStatColours::on_buttons_clicked(QAbstractButton *button)
{
    switch (ui->buttons->standardButton(button))
    {
    case QDialogButtonBox::RestoreDefaults:
        restoreDefaults();
        break;

    case QDialogButtonBox::Reset:
        reset();
        break;

    case QDialogButtonBox::Ok:
        apply();
        accept();
        break;

    case QDialogButtonBox::Apply:
        apply();
        break;

    case QDialogButtonBox::Cancel:
        reset();
        reject();
        break;

    default:;
    }
}

void RazorSysStatColours::restoreDefaults()
{
    bool wereTheSame = mColours == mDefaultColours;

    mColours = mDefaultColours;
    applyColoursToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(!wereTheSame);
}

void RazorSysStatColours::reset()
{
    bool wereTheSame = mColours == mInitialColours;

    mColours = mInitialColours;
    applyColoursToButtons();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(!wereTheSame);
}

void RazorSysStatColours::apply()
{
    emit coloursChanged();

    ui->buttons->button(QDialogButtonBox::Apply)->setEnabled(false);
}

RazorSysStatColours::Colours RazorSysStatColours::colours() const
{
    return mColours;
}

RazorSysStatColours::Colours RazorSysStatColours::defaultColours() const
{
    return mDefaultColours;
}
