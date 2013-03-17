/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2012 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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

#include "razorquicklaunch.h"
#include "quicklaunchbutton.h"
#include <razorqt/razorgridlayout.h>
#include "quicklaunchaction.h"
#include "../panel/irazorpanelplugin.h"

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
#include <QSettings>
#include <QLabel>

RazorQuickLaunch::RazorQuickLaunch(IRazorPanelPlugin *plugin, QWidget* parent) :
    QFrame(parent),
    mPlugin(plugin),
    mPlaceHolder(0)
{
    setAcceptDrops(true);

    mLayout = new RazorGridLayout(this);
    setLayout(mLayout);

    QSettings *settings = mPlugin->settings();
    int count = settings->beginReadArray("apps");

    QString desktop;
    QString file;
    QString execname;
    QString exec;
    QString icon;
    for (int i = 0; i < count; ++i)
    {
        settings->setArrayIndex(i);
        desktop = settings->value("desktop", "").toString();
        file = settings->value("file", "").toString();
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
            execname = settings->value("name", "").toString();
            exec = settings->value("exec", "").toString();
            icon = settings->value("icon", "").toString();
            if (icon.isNull())
            {
                qDebug() << "Icon" << icon << "is not valid (isNull). Skipped.";
                continue;
            }
            addButton(new QuickLaunchAction(execname, exec, icon, this));
        }
    } // for

    settings->endArray();

    if (mLayout->isEmpty())
        showPlaceHolder();

    realign();
}

RazorQuickLaunch::~RazorQuickLaunch()
{
}

int RazorQuickLaunch::indexOfButton(QuickLaunchButton* button) const
{
    return mLayout->indexOf(button);
}

int RazorQuickLaunch::countOfButtons() const
{
    return mLayout->count();
}

void RazorQuickLaunch::realign()
{
    mLayout->setEnabled(false);
    IRazorPanel *panel = mPlugin->panel();

    if (panel->isHorizontal())
    {
        mLayout->setRowCount(panel->lineCount());
        mLayout->setColumnCount(0);
    }
    else
    {
        mLayout->setColumnCount(panel->lineCount());
        mLayout->setRowCount(0);
    }
    mLayout->setEnabled(true);
}

void RazorQuickLaunch::addButton(QuickLaunchAction* action)
{
    mLayout->setEnabled(false);
    QuickLaunchButton* btn = new QuickLaunchButton(action, this);
    mLayout->addWidget(btn);

    connect(btn, SIGNAL(switchButtons(QuickLaunchButton*,QuickLaunchButton*)), this, SLOT(switchButtons(QuickLaunchButton*,QuickLaunchButton*)));
    connect(btn, SIGNAL(buttonDeleted()), this, SLOT(buttonDeleted()));
    connect(btn, SIGNAL(movedLeft()), this, SLOT(buttonMoveLeft()));
    connect(btn, SIGNAL(movedRight()), this, SLOT(buttonMoveRight()));

    mLayout->removeWidget(mPlaceHolder);
    delete mPlaceHolder;
    mPlaceHolder = 0;
    mLayout->setEnabled(true);
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
            qWarning() << "XdgDesktopFile" << fileName << "is not valid";
            QMessageBox::information(this, tr("Drop Error"),
                              tr("File/URL '%1' cannot be embedded into QuickLaunch for now").arg(fileName)
                            );
        }
    }
    saveSettings();
}

void RazorQuickLaunch::switchButtons(QuickLaunchButton *button1, QuickLaunchButton *button2)
{
    if (button1 == button2)
        return;

    int n1 = mLayout->indexOf(button1);
    int n2 = mLayout->indexOf(button2);

    int l = qMin(n1, n2);
    int m = qMax(n1, n2);

    mLayout->moveItem(l, m);
    mLayout->moveItem(m-1, l);
    saveSettings();
}

void RazorQuickLaunch::buttonDeleted()
{
    QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn)
        return;

    mLayout->removeWidget(btn);
    btn->deleteLater();
    saveSettings();

    if (mLayout->isEmpty())
        showPlaceHolder();
}

void RazorQuickLaunch::buttonMoveLeft()
{
    QuickLaunchButton *btn = qobject_cast<QuickLaunchButton*>(sender());
    if (!btn)
        return;

    int index = indexOfButton(btn);
    if (index > 0)
    {
        mLayout->moveItem(index, index - 1);
        saveSettings();
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
        mLayout->moveItem(index, index + 1);
        saveSettings();
    }
}


void RazorQuickLaunch::saveSettings()
{
    QSettings *settings = mPlugin->settings();
    settings->remove("apps");
    settings->beginWriteArray("apps");
    int i = 0;

    for(int j=0; j<mLayout->count(); ++j)
    {
        QuickLaunchButton *b = qobject_cast<QuickLaunchButton*>(mLayout->itemAt(j)->widget());
        if(!b)
            continue;

        settings->setArrayIndex(i);

        QHashIterator<QString,QString> it(b->settingsMap());
        while (it.hasNext())
        {
            it.next();
            settings->setValue(it.key(), it.value());
        }

        ++i;
    }

    settings->endArray();
}

void RazorQuickLaunch::showPlaceHolder()
{
    if (!mPlaceHolder)
    {
        mPlaceHolder = new QLabel(this);
        mPlaceHolder->setObjectName("QuckLaunchPlaceHolder");
        mPlaceHolder->setText(tr("Drop application\nicons here"));
    }

    mLayout->addWidget(mPlaceHolder);
}
