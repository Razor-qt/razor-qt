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
    
    widget.setupUi(this);
    widget.mimetypeIconLabel->setPixmap(m_MimeInfo->icon().pixmap(widget.mimetypeIconLabel->size()));
    widget.mimetypeLabel->setText(m_MimeInfo->comment());
    widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    fillApplicationListWidget();
    connect(widget.buttonBox, SIGNAL(accepted()), this, SLOT(ok()));
    connect(widget.applicationListWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(selectionChanged()));
}

ApplicationChooser::~ApplicationChooser()
{
}

void ApplicationChooser::fillApplicationListWidget()
{
    if (m_MimeInfo) 
    {
        XdgDesktopFile* currentDefaultApplication = XdgDesktopFileCache::getDefaultApp(m_MimeInfo->mimeType());
        QSet<XdgDesktopFile*> addedApps; 
        QList<XdgDesktopFile*> applications; 
    
        for (XdgMimeInfo *mimeInfo = m_MimeInfo; mimeInfo; mimeInfo = XdgMimeInfoCache::xdgMimeInfo(mimeInfo->subClassOf()))
        {
            applications.append(XdgDesktopFileCache::getApps(mimeInfo->mimeType()));
        }
        applications.append(XdgDesktopFileCache::getAllFiles());

        // Insert all found apps in the listwidget, skipping repetitions and putting current default
        // application at the top (if it exists)
        foreach (XdgDesktopFile* desktopFile, applications) 
        {
            if (addedApps.contains(desktopFile)) 
                continue;
            
            // Only applications
            if (desktopFile->type() != XdgDesktopFile::ApplicationType)  
                continue;

            // The application should be able to open documents, so it should accept a file or uri argument, or
            // else we cannot use it
            QString exec = desktopFile->value("Exec").toString();
            if (! ((exec.contains("%f") || exec.contains("%F") || exec.contains("%u") || exec.contains("%U")))) 
                continue;
 
            QListWidgetItem *item = new QListWidgetItem();
            item->setIcon(desktopFile->icon());
            item->setText(desktopFile->name());
            item->setData(32, QVariant::fromValue<XdgDesktopFile*>(desktopFile));
            
            if (desktopFile == currentDefaultApplication) 
            {
                widget.applicationListWidget->insertItem(0, item);
                item->setSelected(true);
            }
            else 
            {
                widget.applicationListWidget->addItem(item);
                item->setSelected(false);
            }

            addedApps.insert(desktopFile);
        }
        selectionChanged();
       
        widget.applicationListWidget->setFocus();
    }
}

void ApplicationChooser::ok()
{
    XdgDesktopFile* desktopfile = widget.applicationListWidget->currentItem()->data(32).value<XdgDesktopFile*>();
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
    widget.appIconLabel->clear();
    widget.appNameLabel->clear();
    widget.handlesLabel->hide();
    widget.mimetypeListWidget->clear(); 
    widget.mimetypeListWidget->setVisible(false);

    QListWidgetItem* newItem = widget.applicationListWidget->currentItem();
    qDebug() << newItem;
    if (newItem) {
        qDebug() << newItem->isSelected();
        qDebug() << newItem->data(32).toInt();
    }
    if (newItem && newItem->data(32).value<XdgDesktopFile*>())
    {
        XdgDesktopFile* desktopFile = newItem->data(32).value<XdgDesktopFile*>();
        
        QIcon icon = desktopFile->icon();
        if (! icon.isNull()) 
        {
            widget.appIconLabel->setPixmap(icon.pixmap(widget.appIconLabel->size()));
        }
        
        widget.appNameLabel->setText(desktopFile->name());

        widget.handlesLabel->show();

        QSet<QString> mimeTypeComments;
        foreach (QString mimeType, desktopFile->value("MimeType").toString().split(";", QString::SkipEmptyParts))
        {
            XdgMimeInfo* mimeInfo = XdgMimeInfoCache::xdgMimeInfo(mimeType);
            if (mimeInfo)
            {
                mimeTypeComments.insert(mimeInfo->comment());
            }
        }
        QList<QString> mimetypeCommentsList = mimeTypeComments.toList();
        qSort(mimetypeCommentsList);
        foreach (QString mimetypeComment, mimetypeCommentsList)
        {
            QListWidgetItem* item = new QListWidgetItem(mimetypeComment, widget.mimetypeListWidget);
            item->setFlags(0);
        }

        widget.mimetypeListWidget->setVisible(true);

        widget.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}