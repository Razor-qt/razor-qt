/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2013 Christian Surlykke
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

#include <QTreeWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QDebug>

#include "libraries/qtxdg/xdgdesktopfile.h"
#include "libraries/qtxdg/xdgmime.h"
#include "applicationchooser.h"


ApplicationChooser::ApplicationChooser(XdgMimeInfo* mimeInfo, QSettings* defaultsList, QObject *parent, QString uri) :
    m_MimeInfo(mimeInfo),
    m_DefaultsList(defaultsList)
{
    
    widget.setupUi(this);
    widget.mimetypeIconLabel->setPixmap(m_MimeInfo->icon().pixmap(widget.mimetypeIconLabel->size()));
    widget.mimetypeLabel->setText(m_MimeInfo->comment());
    widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    fillApplicationListWidget();
    connect(widget.buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(widget.applicationTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this,                           SLOT(selectionChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

ApplicationChooser::~ApplicationChooser()
{
}

void ApplicationChooser::fillApplicationListWidget()
{
    if (m_MimeInfo) 
    {
        XdgDesktopFile* currentDefaultApplication = XdgDesktopFileCache::getDefaultApp(m_MimeInfo->mimeType());
        QList<XdgDesktopFile*> addedApps;

        QList<XdgMimeInfo*> mimeInfos;
        XdgMimeInfo* mimeInfo = m_MimeInfo;

        while (mimeInfo)
        {
            mimeInfos.append(mimeInfo);
            if (! mimeInfo->subClassOf().isEmpty())
            {
                mimeInfo = XdgMimeInfoCache::xdgMimeInfo(mimeInfo->subClassOf());
            }
            else
            {
                mimeInfo = 0;
            }
        }
        mimeInfos.append(0); // Represents 'other mimetypes'. Hackish - I know. 

        QTreeWidgetItem *groupToOpenInitially = 0; 
        QTreeWidgetItem *itemToSelectInitially = 0;

        foreach (XdgMimeInfo* mimeInfo, mimeInfos)
        {
            QString headingText;
            QList<XdgDesktopFile*> tmp; 
            QList<XdgDesktopFile*> appsForThisMimetype;
            
            if (mimeInfo)
            {
                headingText = tr("Applications that handle %1").arg(mimeInfo->comment());
                tmp = XdgDesktopFileCache::getApps(mimeInfo->mimeType());
            }
            else
            {
               headingText = tr("Other applications");
               tmp =  XdgDesktopFileCache::getAllFiles();
            }
            
            qDebug() << headingText << appsForThisMimetype.size() << groupToOpenInitially;
            
            foreach(XdgDesktopFile* df, tmp)
            {
                qDebug() << "Consider:" << df->name() 
                         << "- type:" << df->type() 
                         << "- Exec:" << df->value("Exec");

                if (addedApps.contains(df)) continue;
                if (df->type() != XdgDesktopFile::ApplicationType) continue;
                
                QString exec = df->value("Exec").toString();
                if (! ((exec.contains("%f") || exec.contains("%F") || exec.contains("%u") || exec.contains("%U"))))
                    continue;
                
                appsForThisMimetype.append(df);
                addedApps.append(df);
            }
                
            QTreeWidgetItem *appGroup = new QTreeWidgetItem(widget.applicationTreeWidget);
            appGroup->setText(0, headingText);
            
            // We expand the first group that has applications. These applications
            // would be the most relevant
            if ((!groupToOpenInitially) && appsForThisMimetype.size() > 0)
            {
                groupToOpenInitially = appGroup;
            }

            foreach (XdgDesktopFile* desktopFile, appsForThisMimetype)
            {
                QTreeWidgetItem *item = new QTreeWidgetItem(appGroup);
                item->setIcon(0, desktopFile->icon());
                item->setText(0, desktopFile->name());
                QString filenameNoPath = QFileInfo(desktopFile->fileName()).fileName();
                item->setData(0, 32, filenameNoPath);

                // If there is a default application, we select that and open 
                // its group;
                if (desktopFile == currentDefaultApplication)
                {
                    itemToSelectInitially = item; 
                    groupToOpenInitially = appGroup;
                }

            }

        }

        if (groupToOpenInitially)
        {
            groupToOpenInitially->setExpanded(true);
        }

        if (itemToSelectInitially)
        {
            itemToSelectInitially->setExpanded(true);
        }
        
        widget.applicationTreeWidget->resizeColumnToContents(0);
        widget.applicationTreeWidget->setFocus();
    }
}

void ApplicationChooser::ok()
{
    QString desktopfile = widget.applicationTreeWidget->currentItem()->data(0, 32).toString();
    if (! desktopfile.isEmpty())
    {
        qDebug() << "Ok called, selecting: " << desktopfile;
        m_DefaultsList->beginGroup("Default Applications");
        m_DefaultsList->setValue(m_MimeInfo->mimeType(), desktopfile);
        m_DefaultsList->endGroup();
    }
}

void ApplicationChooser::selectionChanged(QTreeWidgetItem* newItem, QTreeWidgetItem* oldItem)
{
    widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    if (newItem && (! newItem->data(0, 32).toString().isEmpty()))
    {
        widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}