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

#include <QListWidgetItem>
#include <QDebug>
#include <QIcon>
#include <QPixmap>
#include <QListWidget>

#include "qtxdg/xdgmime.h"
#include "qtxdg/xdgdesktopfile.h"

#include "mimetypeviewer.h"
#include "applicationchooser.h"
#include "qtxdg/xdgdirs.h"
#include "razorqt/razorsettings.h"

MimetypeViewer::MimetypeViewer( QWidget *parent) : 
        m_CurrentMime(0)
 {
    widget.setupUi(this);

    initializeMimeTreeWidget();

    connect(widget.mimetypeTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            this, SLOT(currentMimetypeChanged(QTreeWidgetItem*)));

    connect(widget.chooseApplicationsButton, SIGNAL(clicked()), this, SLOT(chooseApplication()));    
    connect(widget.dialogButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonBoxClicked(QAbstractButton*)));

    QString defaultsListPath(XdgDirs::dataHome(true) + "/applications/defaults.list");
    qDebug() << "defaultsListPath:" << defaultsListPath;
    mDefaultsList = new QSettings(defaultsListPath, XdgDesktopFileCache::desktopFileSettingsFormat(), this);
    mSettingsCache = new RazorSettingsCache(mDefaultsList);
    mSettingsCache->loadFromSettings();
}

MimetypeViewer::~MimetypeViewer() {
}

void MimetypeViewer::initializeMimeTreeWidget()
{
    widget.mimetypeTreeWidget->setHeaderLabel("Mimetypes");
    QTreeWidgetItem* firstItem = 0;

    foreach (const QString media, XdgMimeInfoCache::mediatypes())
    {
        QTreeWidgetItem* typeItem = new QTreeWidgetItem(widget.mimetypeTreeWidget);
        typeItem->setText(0, media);

        if (! firstItem)
        {
            firstItem = typeItem;
        }

        foreach (const QString subtype, XdgMimeInfoCache::subtypes(media))
        {
            QTreeWidgetItem* subtypeItem = new QTreeWidgetItem(typeItem);
            subtypeItem->setText(0, subtype);
        }
    }

    firstItem->setSelected(true);
    widget.mimetypeTreeWidget->setFocus();
}

void MimetypeViewer::currentMimetypeChanged(QTreeWidgetItem* newItem)
{
    widget.appIcon->clear();
    widget.iconLabel->clear();
    widget.descriptionLabel->clear();
    widget.patternsLabel->clear();
    widget.applicationLabel->clear();
    widget.chooseApplicationsButton->setEnabled(false);

    widget.patternsGroupBox->hide();
    widget.applicationsGroupBox->hide();

    if (widget.mimetypeTreeWidget->currentItem() && widget.mimetypeTreeWidget->currentItem()->parent())
    {
        QString media = widget.mimetypeTreeWidget->currentItem()->parent()->text(0);
        QString subtype = widget.mimetypeTreeWidget->currentItem()->text(0);
        m_CurrentMime = XdgMimeInfoCache::xdgMimeInfo(media, subtype);
        
        if (!m_CurrentMime)
        {
            return;
        }

        widget.descriptionLabel->setText(m_CurrentMime->comment());
         
        QIcon icon = m_CurrentMime->icon();
        if (! icon.isNull())
        {
            widget.iconLabel->setPixmap(icon.pixmap(widget.iconLabel->size()));
        }

        widget.patternsLabel->setText(m_CurrentMime->patterns().join(" "));

        widget.chooseApplicationsButton->show();

        XdgDesktopFile* defaultApp = XdgDesktopFileCache::getDefaultApp(m_CurrentMime->mimeType());
        if (defaultApp && defaultApp->isValid())
        {
            QString nonLocalizedName = defaultApp->value("Name").toString();
            QString localizedName = defaultApp->localizedValue("Name", nonLocalizedName).toString();
            QIcon appIcon = defaultApp->icon(); 
            widget.appIcon->setPixmap(appIcon.pixmap(widget.iconLabel->size())); 
            widget.applicationLabel->setText(localizedName);
            widget.chooseApplicationsButton->setText(tr("&Change..."));
        }
        else 
        {
            widget.appIcon->setPixmap(QIcon::fromTheme("unknown").pixmap(widget.appIcon->size()));
            widget.applicationLabel->setText(tr("Not defined"));
            widget.chooseApplicationsButton->setText(tr("&Choose..."));
        }

        widget.patternsGroupBox->show();
        widget.applicationsGroupBox->show();
        widget.chooseApplicationsButton->setEnabled(true);

    }
    else 
    {
        m_CurrentMime = 0;
    }
}

void MimetypeViewer::chooseApplication()
{
    if (m_CurrentMime)
    {
        ApplicationChooser(m_CurrentMime, mDefaultsList, this).exec();    
        currentMimetypeChanged(widget.mimetypeTreeWidget->currentItem());
        widget.mimetypeTreeWidget->setFocus();
    }
}

void MimetypeViewer::dialogButtonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::ButtonRole role = widget.dialogButtonBox->buttonRole(button);
    if (role == QDialogButtonBox::ResetRole)
    {
        mSettingsCache->loadToSettings();
        currentMimetypeChanged(widget.mimetypeTreeWidget->currentItem());
    }
    else
    {
        close();
    }   
}

