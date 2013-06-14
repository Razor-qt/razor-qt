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
#include <QSortFilterProxyModel>
#include <QtConcurrentRun>
#include <QDateTime>

#include "qtxdg/xdgmime.h"
#include "qtxdg/xdgdesktopfile.h"
#include "qtxdg/xdgdirs.h"
#include "razorqt/razorsettings.h"


#include "mimetypeviewer.h"
#include "applicationchooser.h"
#include "mimetypeitemmodel.h"
#include "busyindicator.h"

void initializeMimeInfoCache()
{
    XdgMimeInfoCache::mediatypes();
}

MimetypeViewer::MimetypeViewer( QWidget *parent) : 
        m_MimetypeFilterItemModel(this),
        m_CurrentMime(0),
        mFutureWatcher(0),
        mBusyIndicator(0)

 {
    widget.setupUi(this);
    addSearchIcon();
    widget.searchTermLineEdit->setEnabled(false);

    // initialize XdgMimeInfoCache asynchronously while putting a busyindicator on the mimeTypeTreeview
    mBusyIndicator = new BusyIndicator(widget.mimetypeTreeView);
    mFutureWatcher = new QFutureWatcher<void>(this);
    connect(mFutureWatcher, SIGNAL(finished()), this, SLOT(initializeMimetypeTreeView()));
    mFutureWatcher->setFuture(QtConcurrent::run(initializeMimeInfoCache));
    
    connect(widget.searchTermLineEdit, SIGNAL(textChanged(const QString&)), 
            &m_MimetypeFilterItemModel, SLOT(setFilterFixedString(const QString&)));
    
    connect(widget.searchTermLineEdit, SIGNAL(textChanged(const QString&)), 
            this, SLOT(autoExpandOnSearch()));
            

    connect(widget.chooseApplicationsButton, SIGNAL(clicked()), this, SLOT(chooseApplication()));    
    connect(widget.dialogButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonBoxClicked(QAbstractButton*)));

    QString defaultsListPath(XdgDirs::dataHome(true) + "/applications/defaults.list");
    mDefaultsList = new QSettings(defaultsListPath, XdgDesktopFileCache::desktopFileSettingsFormat(), this);
    mSettingsCache = new RazorSettingsCache(mDefaultsList);
    mSettingsCache->loadFromSettings();
}

MimetypeViewer::~MimetypeViewer() {
}

void MimetypeViewer::addSearchIcon()
{
    QIcon searchIcon = QIcon::fromTheme("system-search");
    if (searchIcon.isNull())
        return;

    widget.searchTermLineEdit->setTextMargins(0, 0, 30, 0);
    QHBoxLayout *hBoxLayout = new QHBoxLayout(widget.searchTermLineEdit);
    hBoxLayout->setContentsMargins(0,0,0,0);
    widget.searchTermLineEdit->setLayout(hBoxLayout);
    QLabel *searchIconLabel = new QLabel(widget.searchTermLineEdit);
    searchIconLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    searchIconLabel->setMinimumHeight(30);
    searchIconLabel->setMinimumWidth(30);

    searchIconLabel->setPixmap(searchIcon.pixmap(QSize(20,20)));
    hBoxLayout->addWidget(searchIconLabel, 0, Qt::AlignRight | Qt::AlignVCenter);
}


void MimetypeViewer::initializeMimetypeTreeView()
{
    m_MimetypeFilterItemModel.setSourceModel(new MimetypeItemModel(&m_MimetypeFilterItemModel));

    widget.mimetypeTreeView->setModel(&m_MimetypeFilterItemModel);
    currentMimetypeChanged();
    widget.mimetypeTreeView->setFocus();
    widget.searchTermLineEdit->setEnabled(true);
    
    connect(widget.mimetypeTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), 
            this, SLOT(currentMimetypeChanged()));

    delete mBusyIndicator;
    mBusyIndicator = 0;
}

void MimetypeViewer::resizeEvent(QResizeEvent* event)
{
    if (mBusyIndicator)
        mBusyIndicator->resize(widget.mimetypeTreeView->size());
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
   
    if (!index.isValid()) 
    {
        return;
    }
    
    QVariant variant = index.data(MimeInfoRole);

    m_CurrentMime =  variant.value<XdgMimeInfo*>();
            
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

void MimetypeViewer::autoExpandOnSearch()
{
    for (int i = 0; i < m_MimetypeFilterItemModel.rowCount(); i++) 
    {
        QModelIndex mediatypeIndex = m_MimetypeFilterItemModel.index(i, 0);
        if (m_MimetypeFilterItemModel.rowCount(mediatypeIndex) < 8)
        {
            widget.mimetypeTreeView->setExpanded(mediatypeIndex, true);
        }
    }
}


void MimetypeViewer::chooseApplication()
{
    if (m_CurrentMime)
    {
        ApplicationChooser applicationChooser(m_CurrentMime);
        if (applicationChooser.exec() && applicationChooser.DefaultApplication())
        {
            QString fileNameNoPath = QFileInfo(applicationChooser.DefaultApplication()->fileName()).fileName();
            mDefaultsList->beginGroup("Default Applications");
            mDefaultsList->setValue(m_CurrentMime->mimeType(), fileNameNoPath);
            mDefaultsList->endGroup();   
            currentMimetypeChanged();
        }
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

