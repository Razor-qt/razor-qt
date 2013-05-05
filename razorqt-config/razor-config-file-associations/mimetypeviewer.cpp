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
#include "qtxdg/xdgdirs.h"
#include "razorqt/razorsettings.h"


#include "mimetypeviewer.h"
#include "applicationchooser.h"
#include "mimetypeitemmodel.h"

MimetypeViewer::MimetypeViewer( QWidget *parent) : 
        m_CurrentMime(0)
 {
    widget.setupUi(this);

    initializeMimeTreeWidget();

    connect(widget.mimetypeTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            this, SLOT(currentMimetypeChanged()));

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
    widget.mimetypeTreeView->setModel(new MimetypeItemModel());
    widget.mimetypeTreeView->setFocus();
}

void MimetypeViewer::currentMimetypeChanged()
{
    widget.iconLabel->hide();
    widget.descriptionLabel->setText(tr("None"));
    widget.mimetypeGroupBox->setEnabled(false);
    
    widget.patternsLabel->clear();
    widget.patternsGroupBox->setEnabled(false);
     
    widget.appIcon->hide();
    widget.applicationLabel->clear();
    widget.applicationsGroupBox->setEnabled(false);

    m_CurrentMime = 0;
   
    QModelIndex index = widget.mimetypeTreeView->selectionModel()->currentIndex();

    m_CurrentMime =  index.model()->data(index, MimeInfoRole).value<XdgMimeInfo*>();
            
    if (!m_CurrentMime)
    {
        return;
    }

    widget.descriptionLabel->setText(m_CurrentMime->comment());
    
    QIcon icon = m_CurrentMime->icon();
    if (! icon.isNull())
    {
        widget.iconLabel->setPixmap(icon.pixmap(widget.iconLabel->size()));
        widget.iconLabel->show();
    }
    widget.mimetypeGroupBox->setEnabled(true);


    widget.patternsLabel->setText(m_CurrentMime->patterns().join("  "));
    widget.patternsGroupBox->setEnabled(true);
    
    XdgDesktopFile* defaultApp = XdgDesktopFileCache::getDefaultApp(m_CurrentMime->mimeType());
    if (defaultApp && defaultApp->isValid())
    {
        QString nonLocalizedName = defaultApp->value("Name").toString();
        QString localizedName = defaultApp->localizedValue("Name", nonLocalizedName).toString();
        QIcon appIcon = defaultApp->icon(); 
        widget.appIcon->setPixmap(appIcon.pixmap(widget.iconLabel->size())); 
        widget.appIcon->show();
        widget.applicationLabel->setText(localizedName);
        widget.chooseApplicationsButton->setText(tr("&Change..."));
    }
    else 
    {
        widget.applicationLabel->setText(tr("None"));
        widget.chooseApplicationsButton->setText(tr("&Choose..."));
    }
    widget.applicationsGroupBox->setEnabled(true);

}

void MimetypeViewer::chooseApplication()
{
    if (m_CurrentMime)
    {
        ApplicationChooser(m_CurrentMime, mDefaultsList, this).exec();    
        currentMimetypeChanged();
        widget.mimetypeTreeView->setFocus();
    }
}

void MimetypeViewer::dialogButtonBoxClicked(QAbstractButton* button)
{
    QDialogButtonBox::ButtonRole role = widget.dialogButtonBox->buttonRole(button);
    if (role == QDialogButtonBox::ResetRole)
    {
        mSettingsCache->loadToSettings();
        currentMimetypeChanged();
    }
    else
    {
        close();
    }   
}

