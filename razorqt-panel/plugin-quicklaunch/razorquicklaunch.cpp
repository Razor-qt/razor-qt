/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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


#include "razorquicklaunch.h"
#include "quicklaunchbutton.h"
#include "quicklaunchlayout.h"
#include "quicklaunchaction.h"

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgmime.h>

#include <QtDebug>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtCore/QFileInfo>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QToolButton>
#include <QtGui/QMessageBox>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileIconProvider>


EXPORT_RAZOR_PANEL_PLUGIN_CPP(RazorQuickLaunch)


RazorQuickLaunch::RazorQuickLaunch(const RazorPanelPluginStartInfo* startInfo, QWidget* parent)
    : RazorPanelPlugin(startInfo, parent),
      m_maxIndex(0)
{
    setObjectName("QuickLaunch");
    setAcceptDrops(true);

    delete layout();
    m_layout = new QuickLaunchLayout(this, panel());
    m_layout->setAlignment(Qt::AlignCenter);
    setLayout(m_layout);

    QSettings *s = &settings();
    int count = s->beginReadArray("apps");

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;
    for (int i = 0; i < count; ++i)
    {
        s->setArrayIndex(i);
        desktop = s->value("desktop", "").toString();
        file = s->value("file", "").toString();
        if (! desktop.isEmpty())
        {
            XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(desktop);
            if (!xdg->isValid())
            {
                qDebug() << "XdgDesktopFile" << desktop << "is not valid";
                continue;
            }
            addButton(new QuickLaunchAction(xdg, this));
        }
        else if (! file.isEmpty())
        {
            addButton(new QuickLaunchAction(file, this));
        }
        else
        {
            execname = s->value("name", "").toString();
            exec = s->value("exec", "").toString();
            icon = s->value("icon", "").toString();
            if (icon.isNull())
            {
                qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
                continue;
            }
            addButton(new QuickLaunchAction(execname, exec, icon, this));
        }
    } // for

    s->endArray();
}

RazorQuickLaunch::~RazorQuickLaunch()
{
}

int RazorQuickLaunch::indexOfButton(QuickLaunchButton* button) const
{
    return m_layout->indexOf(button);
}

int RazorQuickLaunch::countOfButtons() const
{
    return m_layout->count();
}

void RazorQuickLaunch::addButton(QuickLaunchAction* action)
{
    QuickLaunchButton* btn = new QuickLaunchButton(m_maxIndex, action, this);
    m_layout->addWidget(btn);
    m_buttons[m_maxIndex] = btn;
    
    connect(btn, SIGNAL(switchButtons(int,int)), this, SLOT(switchButtons(int,int)));
    connect(btn, SIGNAL(buttonDeleted(int)), this, SLOT(buttonDeleted(int)));
    connect(btn, SIGNAL(movedLeft()), this, SLOT(buttonMoveLeft()));
    connect(btn, SIGNAL(movedRight()), this, SLOT(buttonMoveRight()));

    ++m_maxIndex;
}

void RazorQuickLaunch::dragEnterEvent(QDragEnterEvent *e)
{
    // Getting URL from mainmenu...
    if (e->mimeData()->hasUrls())
    {
        e->acceptProposedAction();
        return;
    }

    if (e->source() && e->source()->parent() == this)
    {
        e->acceptProposedAction();
    }
}

void RazorQuickLaunch::dropEvent(QDropEvent *e)
{
    const QMimeData *mime = e->mimeData();
    // duplicates storage: [quicklaunch] issue dragging from qtfm, #252
    QList<QUrl> duplicates;
    // urls from mainmenu
    foreach (QUrl url, mime->urls())
    {
	if (duplicates.contains(url))
            continue;
	else
	    duplicates << url;

        QString fileName(url.toLocalFile());
        XdgDesktopFile * xdg = XdgDesktopFileCache::getFile(fileName);
        QFileInfo fi(fileName);
        qDebug() << fileName << fi.exists() << fi.isExecutable();
        if (xdg->isValid())
        {
            addButton(new QuickLaunchAction(xdg, this));
        }
        else if (fi.exists() && fi.isExecutable() && !fi.isDir())
        {
            addButton(new QuickLaunchAction(fileName, fileName, "", this));
        }
        else if (fi.exists())
        {
            addButton(new QuickLaunchAction(fileName, this));
        }
        else
        {
            qDebug() << "XdgDesktopFile" << fileName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                              tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                            );
        }
    }
    saveSettings();
}

void RazorQuickLaunch::switchButtons(int first, int second)
{
    QuickLaunchButton *w1 = m_buttons[first];
    QuickLaunchButton *w2 = m_buttons[second];
    m_layout->swapButtons(w1, w2);
    saveSettings();
}

void RazorQuickLaunch::buttonDeleted(int id)
{
    QuickLaunchButton *b = m_buttons[id];
    m_layout->removeWidget(b);
    m_buttons.remove(id);
    b->deleteLater();
    saveSettings();
}

void RazorQuickLaunch::buttonMoveLeft()
{
    QuickLaunchButton *btn1 = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn1)
        return;

    int index = indexOfButton(btn1);
    if (index > 0)
    {
        switchButtons(index, index - 1);
    }
}


void RazorQuickLaunch::buttonMoveRight()
{
    QuickLaunchButton *btn1 = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn1)
        return;

    int index = indexOfButton(btn1);
    if (index < countOfButtons() - 1)
    {
        switchButtons(index, index + 1);
    }
}


void RazorQuickLaunch::saveSettings()
{
    settings().remove("apps");
    settings().beginWriteArray("apps");
    int i = 0;

    foreach(QuickLaunchButton *b, m_layout->buttons())
    {
        settings().setArrayIndex(i);

        QHashIterator<QString,QString> it(b->settingsMap());
        while (it.hasNext())
        {
            it.next();
            settings().setValue(it.key(), it.value());
        }
        
        ++i;
    }

    settings().endArray();
}
