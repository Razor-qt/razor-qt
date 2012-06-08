/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QButtonGroup>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

#include "notificationwidgets.h"

#include <QtDebug>


NotificationActionsWidget::NotificationActionsWidget(const QStringList& actions, QWidget *parent)
    : QWidget(parent),
      m_comboBox(0)
{
    QHash<uint,QString> actionMap;

    bool ok;
    for (int i = 0; i < actions.count(); ++i)
    {
        uint id = actions.at(i).toInt(&ok);
        if (!ok)
        {
            qDebug() << "NotificationActionsWidget id has to be number, got:" << actions.at(i) << "Actions:" << actions;
        }
        else if (i == actions.count()-1)
        {
            qDebug() << "NotificationActionsWidget actions has contains pairs (id, value, id, value...) got odd count:" << actions.count() << "Actions:" << actions;
        }
        else
        {
            actionMap[id] = actions.at(i+1);
        }
        ++i; // move to the next ID
    }

    qDebug() << "NotificationActionsWidget processed actions:" << actionMap;

    // Let's be a little bit tricky here. Let's allow only few
    // buttons in the layout. We will use a combobox if there
    // are more actions. I think it's more user friendly.
    QHashIterator<uint,QString> it(actionMap);
    QHBoxLayout *l = new QHBoxLayout();
    setLayout(l);
    if (actionMap.count() < 4)
    {
        QButtonGroup *group = new QButtonGroup(this);

        while (it.hasNext())
        {
            it.next();
            QPushButton *b = new QPushButton(it.value(), this);
            l->addWidget(b);
            group->addButton(b, it.key());
        }
        connect(group, SIGNAL(buttonClicked(int)),
                this, SIGNAL(actionTriggered(uint)));
    }
    else
    {
        l->addWidget(new QLabel(tr("Actions:"), this));
        m_comboBox = new QComboBox(this);
        while (it.hasNext())
        {
            it.next();
            m_comboBox->addItem(it.value(), it.key());
        }
        l->addWidget(m_comboBox);
        QPushButton *b = new QPushButton(tr("OK"), this);
        connect(b, SIGNAL(clicked()),
                this, SLOT(actionComboBoxActivated()));
    }
}

void NotificationActionsWidget::actionComboBoxActivated()
{
    if (!m_comboBox)
        return;
    int ix = m_comboBox->currentIndex();
    if (ix == -1)
        return;
    emit actionTriggered(m_comboBox->itemText(ix));
}
