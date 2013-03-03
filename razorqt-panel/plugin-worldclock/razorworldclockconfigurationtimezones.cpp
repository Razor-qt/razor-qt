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


#include <unicode/timezone.h>
#include <unicode/calendar.h>

#include "razorworldclockconfigurationtimezones.h"
#include "ui_razorworldclockconfigurationtimezones.h"


RazorWorldClockConfigurationTimeZones::RazorWorldClockConfigurationTimeZones(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorWorldClockConfigurationTimeZones)
{
    setObjectName("WorldClockConfigurationTimeZonesWindow");
    ui->setupUi(this);

    connect(ui->timeZonesTW, SIGNAL(itemSelectionChanged()), SLOT(itemSelectionChanged()));
    connect(ui->timeZonesTW, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(itemDoubleClicked(QTreeWidgetItem*,int)));
}

RazorWorldClockConfigurationTimeZones::~RazorWorldClockConfigurationTimeZones()
{
    delete ui;
}

QString RazorWorldClockConfigurationTimeZones::timeZone()
{
    return mTimeZone;
}

void RazorWorldClockConfigurationTimeZones::itemSelectionChanged()
{
    QList<QTreeWidgetItem*> items = ui->timeZonesTW->selectedItems();
    if (!items.empty())
        mTimeZone = items[0]->data(1, Qt::UserRole).toString();
    else
        mTimeZone.clear();
}

void RazorWorldClockConfigurationTimeZones::itemDoubleClicked(QTreeWidgetItem* /*item*/, int /*column*/)
{
    if (!mTimeZone.isEmpty())
        accept();
}

int RazorWorldClockConfigurationTimeZones::updateAndExec()
{
    ui->timeZonesTW->clear();

    UDate uDate = Calendar::getNow();

    StringEnumeration* allTimeZones = TimeZone::createEnumeration();
    UErrorCode uErrorCode = U_ZERO_ERROR;
    while (const UnicodeString* uString = allTimeZones->snext(uErrorCode))
    {
        if (uErrorCode == U_ZERO_ERROR)
        {
            QString timeZoneOffset;
            {
                TimeZone *timeZone = TimeZone::createTimeZone(*uString);
                UnicodeString uControlID;
                timeZone->getID(uControlID);
                if (uControlID == *uString)
                {
                    int32_t rawOffset;
                    int32_t dstOffset;
                    timeZone->getOffset(uDate, false, rawOffset, dstOffset, uErrorCode);
                    int minOffset = (rawOffset + dstOffset) / 60000;
                    QChar sign = (minOffset >= 0) ? '+' : '-';
                    int hourOffset = qAbs(minOffset / 60);
                    minOffset = qAbs(minOffset) % 60;
                    timeZoneOffset = QString("%1%2:%3").arg(sign).arg(hourOffset, 2, 10, QChar('0')).arg(minOffset, 2, 10, QChar('0'));
                }
                delete timeZone;
            }

            QString qString(QString::fromUtf16(uString->getBuffer()));
            QStringList qStrings(qString.split('/'));

            if ((qStrings[0] == "Brazil") || (qStrings[0] == "Canada") || (qStrings[0] == "Chile") || (qStrings[0] == "Cuba") || (qStrings[0] == "Jamaica") || (qStrings[0] == "Mexico") || (qStrings[0] == "Navajo") || (qStrings[0] == "US"))
                qStrings.prepend("America");
            else if (qStrings[0] == "Egypt")
                qStrings.prepend("Africa");
            else if ((qStrings[0] == "Mideast") || (qStrings[0] == "Hongkong") || (qStrings[0] == "Indian") || (qStrings[0] == "Iran") || (qStrings[0] == "Israel") || (qStrings[0] == "Japan") || (qStrings[0] == "Libya") || (qStrings[0] == "Singapore") || (qStrings[0] == "Turkey"))
                qStrings.prepend("Asia");
            else if ((qStrings[0] == "Eire") || (qStrings[0] == "GB-Eire") || (qStrings[0] == "Greenwich") || (qStrings[0] == "Iceland") || (qStrings[0] == "Poland") ||(qStrings[0] == "Portugal"))
                qStrings.prepend("Europe");
            else if (qStrings[0] == "Kwajalein")
                qStrings.prepend("Pacific");
            else if ((qStrings[0] == "Etc") || (qStrings[0] == "SystemV"))
                qStrings.prepend("Other");

            if (qStrings.size() == 1)
                qStrings.prepend("Other");

            QTreeWidgetItem *item = NULL;

            int m = qStrings.size() - 1;
            for (int i = 0; i != m; ++i)
            {
                QList<QTreeWidgetItem *> items = ui->timeZonesTW->findItems(qStrings[i], Qt::MatchExactly | Qt::MatchRecursive);

                QTreeWidgetItem *newItem = NULL;

                int n = items.size();
                for (int j = 0; j != n; ++j)
                {
                    if (items[j]->data(0, Qt::UserRole) != i)
                        continue;

                    newItem = items[j];
                    QTreeWidgetItem *parentItem = items[j]->parent();
                    for (int k = i - 1 ; k >= 0 ; --k, parentItem = parentItem->parent())
                    {
                        if (parentItem->text(0) != qStrings[k])
                        {
                            newItem = NULL;
                            break;
                        }
                    }

                    if (newItem)
                        break;
                }

                if (!newItem)
                {
                    newItem = new QTreeWidgetItem(QStringList() << qStrings[i]);
                    newItem->setData(0, Qt::UserRole, i);
                    if (!item)
                        ui->timeZonesTW->addTopLevelItem(newItem);
                    else
                        item->addChild(newItem);
                }
                item = newItem;
            }

            QTreeWidgetItem *tzItem = new QTreeWidgetItem(QStringList() << qStrings[qStrings.size() - 1] << timeZoneOffset);
            tzItem->setData(0, Qt::UserRole, m);
            tzItem->setData(1, Qt::UserRole, qString);
            item->addChild(tzItem);
        }
    }

    delete allTimeZones;

    ui->timeZonesTW->sortByColumn(0, Qt::AscendingOrder);

    return exec();
}
