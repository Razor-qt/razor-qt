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

#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QString>
#include <QDebug>

#include "libraries/qtxdg/xdgdesktopfile.h"
#include "libraries/qtxdg/xdgmime.h"
#include "applicationchooser.h"

Q_DECLARE_METATYPE(XdgDesktopFile*)

ApplicationChooser::ApplicationChooser(XdgMimeInfo* mimeInfo, QSettings* defaultsList, QObject *parent, QString uri) :
    m_MimeInfo(mimeInfo),
    m_DefaultsList(defaultsList)
{
    m_CurrentDefaultApplication = XdgDesktopFileCache::getDefaultApp(m_MimeInfo->mimeType());
    widget.setupUi(this);
    widget.mimetypeIconLabel->setPixmap(m_MimeInfo->icon().pixmap(widget.mimetypeIconLabel->size()));
    widget.mimetypeLabel->setText(m_MimeInfo->comment());
    widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    fillApplicationListWidget();
    connect(widget.buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(widget.applicationTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(selectionChanged()));
}

ApplicationChooser::~ApplicationChooser()
{
}


void ApplicationChooser::fillApplicationListWidget()
{
    widget.applicationTreeWidget->clear();
    if (m_MimeInfo) 
    {
        QSet<XdgDesktopFile*> addedApps; 
        QList<XdgDesktopFile*> applicationsThatHandleThisMimetype = XdgDesktopFileCache::getApps(m_MimeInfo->mimeType());
        QList<XdgDesktopFile*> otherApplications;
       
        for (XdgMimeInfo* mimeInfo = XdgMimeInfoCache::xdgMimeInfo(m_MimeInfo->subClassOf());
             mimeInfo; 
             mimeInfo =XdgMimeInfoCache::xdgMimeInfo(mimeInfo->subClassOf()))
        {
            otherApplications.append(XdgDesktopFileCache::getApps(mimeInfo->mimeType()));
        }
        otherApplications.append(XdgDesktopFileCache::getAllFiles());

        qDebug() << "Adding applications1, size:"  << applicationsThatHandleThisMimetype.size();
        QTreeWidgetItem* headingItem = new QTreeWidgetItem(widget.applicationTreeWidget);
        headingItem->setExpanded(true);
        headingItem->setFlags(Qt::ItemIsEnabled);
        headingItem->setText(0, tr("Applications that handle %1").arg(m_MimeInfo->comment()));
        QSize size(0, 25);
        headingItem->setSizeHint(0, size);
        

        if (applicationsThatHandleThisMimetype.isEmpty()) 
        {
            QTreeWidgetItem* noAppsFoundItem = new QTreeWidgetItem(headingItem);
            noAppsFoundItem->setText(0, tr("No applications found"));
            noAppsFoundItem->setFlags(0);
            QFont font = noAppsFoundItem->font(0);
            font.setStyle(QFont::StyleItalic);
            noAppsFoundItem->setFont(0, font);
        }
        else 
        {
            addApplicationsToApplicationListWidget(headingItem, applicationsThatHandleThisMimetype, addedApps);
        }
      
        qDebug() << "Adding other applications:" << otherApplications.size();
        headingItem = new QTreeWidgetItem(widget.applicationTreeWidget);
        headingItem->setFlags(Qt::ItemIsEnabled);
        headingItem->setExpanded(true);
        headingItem->setText(0, tr("Other applications"));
        headingItem->setSizeHint(0, size);
        addApplicationsToApplicationListWidget(headingItem, otherApplications, addedApps);

        widget.applicationTreeWidget->setFocus();
    }
}

void ApplicationChooser::addApplicationsToApplicationListWidget(QTreeWidgetItem* parent, 
                                                                QList<XdgDesktopFile*> applications, 
                                                                QSet<XdgDesktopFile*>& alreadyAdded)
{
    // Insert applications in the listwidget, skipping already added applications
    foreach (XdgDesktopFile* desktopFile, applications) 
    {
        if (alreadyAdded.contains(desktopFile)) 
            continue;
        
        // Only applications
        if (desktopFile->type() != XdgDesktopFile::ApplicationType)  
            continue;

        // The application should be able to open documents, so it should accept a file or uri argument, or
        // else we cannot use it
        QString exec = desktopFile->value("Exec").toString();
        if (! ((exec.contains("%f") || exec.contains("%F") || exec.contains("%u") || exec.contains("%U")))) 
            continue;

        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        item->setIcon(0, desktopFile->icon());
        item->setText(0, desktopFile->name());
        item->setData(0, 32, QVariant::fromValue<XdgDesktopFile*>(desktopFile));
        item->setSelected(desktopFile == m_CurrentDefaultApplication);

        alreadyAdded.insert(desktopFile);
    }
    selectionChanged();
}



void ApplicationChooser::ok()
{
    XdgDesktopFile* desktopfile = widget.applicationTreeWidget->currentItem()->data(0, 32).value<XdgDesktopFile*>();
    if (desktopfile)
    {
        qDebug() << "Ok called, selecting: " << desktopfile;
        QString fileNameNoPath = QFileInfo(desktopfile->fileName()).fileName();
        m_DefaultsList->beginGroup("Default Applications");
        m_DefaultsList->setValue(m_MimeInfo->mimeType(), fileNameNoPath);
        m_DefaultsList->endGroup();
    }
}

void ApplicationChooser::selectionChanged()
{
    widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    QTreeWidgetItem* newItem = widget.applicationTreeWidget->currentItem();
    if (newItem && newItem->data(0, 32).value<XdgDesktopFile*>())
    {
        widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}