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

#include <QtCore/QDirIterator>
#include <QtGui/QLineEdit>

#include "mainwindow.h"
#include <QtDebug>

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razoraboutdlg.h>

#include "qcategorizedview.h"
#include "qcategorydrawer.h"
#include "qcategorizedsortfilterproxymodel.h"

namespace RazorConfig {

struct ConfigPaneData: public QSharedData
{
    QString id;
    QString category;
    XdgDesktopFile *xdg;
};

class ConfigPane
{
public:
    ConfigPane(): d(new ConfigPaneData) { }
    ConfigPane(const ConfigPane &other): d(other.d) { }

    inline QString &id() const { return d->id; }
    inline XdgDesktopFile* xdg() const { return d->xdg; }
    inline void setXdg(XdgDesktopFile* xdg) { d->xdg = xdg; }
    inline QString &category() const { return d->category; }

    bool operator==(const ConfigPane &other)
    {
        return d->id == other.id();
    }

private:
    QExplicitlySharedDataPointer<ConfigPaneData> d;
};

static QStringList preferredCategoryOrder;

static bool categorySorter(const ConfigPane &a, const ConfigPane &b)
{
    int aIdx = preferredCategoryOrder.indexOf(a.category());
    int bIdx = preferredCategoryOrder.indexOf(b.category());
    if (aIdx < 0)
        return false;
    return aIdx < bIdx;
}

class ConfigPaneModel: public QAbstractListModel
{
public:
    ConfigPaneModel(): QAbstractListModel()
    {
        if (preferredCategoryOrder.isEmpty()) {
            preferredCategoryOrder << "Razor" << "System" << "Other";
        }
        
        QDirIterator it("/usr/share/applications", QStringList() << "*.desktop", QDir::NoFilter, QDirIterator::Subdirectories);
        QString name;
        QString categories;
        QString category;
        QString onlyShowIn;
    
        while (it.hasNext()) {
            name = it.next();
            XdgDesktopFile *xdg = new XdgDesktopFile();
            xdg->load(name);
            if (!xdg->isValid())
            {
                qDebug() << "INVALID DESKTOP FILE:" << name;
                delete xdg;
                continue;
            }

            onlyShowIn = xdg->value("OnlyShowIn").toString();
            if (!onlyShowIn.isEmpty()
                && !onlyShowIn.contains("X-RAZOR") && !onlyShowIn.contains("RAZOR")
                //&& !onlyShowIn.contains("YaST")
            )
            {
                qDebug() << "NOT SHOWN" << name << onlyShowIn;
                delete xdg;
                continue;
            }
        
            // do not show self
            if (xdg->value("Exec").toString() == "razor-config")
            {
                delete xdg;
                continue;
            }

            categories = xdg->value("Categories").toString();
            if (!categories.contains("Settings"))
            {
                delete xdg;
                continue;
            }
        
            if (categories.contains("X-RAZOR") || categories.contains("RAZOR"))
            {
                category = "Razor";
            }
            else if (categories.contains("System"))
            {
                category = "System";
            }
            else
            {
                category = "Other";
            }
            
            ConfigPane pane;
            pane.id() = xdg->value("Icon").toString();
            pane.category() = category;
            pane.setXdg(xdg);
            m_list.append(pane);
        }

        qSort(m_list.begin(), m_list.end(), categorySorter);
    }
    
    void activateItem(const QModelIndex &index)
    {
        if (!index.isValid())
            return;
        m_list[index.row()].xdg()->startDetached();
    }

    ~ConfigPaneModel() { }

    int rowCount(const QModelIndex &parent = QModelIndex()) const
    {
        return m_list.count();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
    {
        return false;
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (role == Qt::DisplayRole)
            return m_list[index.row()].xdg()->name();
        if (role == QCategorizedSortFilterProxyModel::CategoryDisplayRole)
            return m_list[index.row()].category();
        if (role == QCategorizedSortFilterProxyModel::CategorySortRole)
            return m_list[index.row()].category();
        if (role == Qt::UserRole)
            return m_list[index.row()].id();
        if (role == Qt::DecorationRole)
        {
            return m_list[index.row()].xdg()->icon();
        }
        return QVariant();
    }

private:
    QList<ConfigPane> m_list;
};
    
}


RazorConfig::MainWindow::MainWindow() : QMainWindow()
{
    setupUi(this);
    
    model = new ConfigPaneModel();

    view->setViewMode(QListView::IconMode);
    view->setIconSize(QSize(48, 48));
    view->setGridSize(QSize(140, 74));
    view->setCategoryDrawer(new QCategoryDrawerV3(view));

    proxyModel = new QCategorizedSortFilterProxyModel();
    proxyModel->setCategorizedModel(true);
    proxyModel->setSourceModel(model);

    view->setModel(proxyModel);
    
    connect(view, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(activateItem(const QModelIndex&)));
}

void RazorConfig::MainWindow::activateItem(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QModelIndex orig = proxyModel->mapToSource(index);
    model->activateItem(orig);
}
