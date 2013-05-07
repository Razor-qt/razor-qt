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
#include <QTimer>

#include "mainwindow.h"
#include <QtDebug>
#include <QtGui/QMessageBox>
#include <QtGui/QStyledItemDelegate>

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <qtxdg/xdgmenu.h>
#include <qtxdg/xmlhelper.h>

#include "qcategorizedview.h"
#include "qcategorydrawer.h"
#include "qcategorizedsortfilterproxymodel.h"

namespace RazorConfig {

struct ConfigPaneData: public QSharedData
{
    QString id;
    QString category;
    XdgDesktopFile xdg;
};

class ConfigPane
{
public:
    ConfigPane(): d(new ConfigPaneData) { }
    ConfigPane(const ConfigPane &other): d(other.d) { }

    inline QString &id() const { return d->id; }
    inline XdgDesktopFile xdg() const { return d->xdg; }
    inline void setXdg(XdgDesktopFile xdg) { d->xdg = xdg; }
    inline QString &category() const { return d->category; }

    bool operator==(const ConfigPane &other)
    {
        return d->id == other.id();
    }

private:
    QExplicitlySharedDataPointer<ConfigPaneData> d;
};


class ConfigPaneModel: public QAbstractListModel
{
public:
    ConfigPaneModel(): QAbstractListModel()
    {
        QString menuFile = XdgMenu::getMenuFileName("config.menu");
        XdgMenu xdgMenu;
        xdgMenu.setEnvironments(QStringList() << "X-RAZOR" << "Razor");
        bool res = xdgMenu.read(menuFile);
        if (!res)
        {
            QMessageBox::warning(0, "Parse error", xdgMenu.errorString());
            return;
        }

        DomElementIterator it(xdgMenu.xml().documentElement() , "Menu");
        while(it.hasNext())
        {
            this->builGroup(it.next());
        }
    }

    void builGroup(const QDomElement& xml)
    {
        QString category;
        if (! xml.attribute("title").isEmpty())
            category = xml.attribute("title");
        else
            category = xml.attribute("name");

        DomElementIterator it(xml , "AppLink");
        while(it.hasNext())
        {
            QDomElement x = it.next();

            XdgDesktopFile xdg;
            xdg.load(x.attribute("desktopFile"));

            ConfigPane pane;
            pane.id() = xdg.value("Icon").toString();
            pane.category() = category;
            pane.setXdg(xdg);
            m_list.append(pane);
        }
    }

    void activateItem(const QModelIndex &index)
    {
        if (!index.isValid())
            return;
        m_list[index.row()].xdg().startDetached();
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
        if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
            return m_list[index.row()].xdg().name();
        if (role == QCategorizedSortFilterProxyModel::CategoryDisplayRole)
            return m_list[index.row()].category();
        if (role == QCategorizedSortFilterProxyModel::CategorySortRole)
            return m_list[index.row()].category();
        if (role == Qt::UserRole)
            return m_list[index.row()].id();
        if (role == Qt::DecorationRole)
        {
            return m_list[index.row()].xdg().icon(XdgIcon::defaultApplicationIcon());
        }
        return QVariant();
    }

private:
    QList<ConfigPane> m_list;
};

}


class ConfigItemDelegate : public QStyledItemDelegate
{
public:
    ConfigItemDelegate(QCategorizedView* view) : mView(view) { }
    ~ConfigItemDelegate() { }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        int height = QStyledItemDelegate::sizeHint(option, index).height();
        return QSize(mView->gridSize().width(), qMin(height, mView->gridSize().height()));
    }

protected:
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyleOptionViewItemV4 opt = option;
        initStyleOption(&opt, index);

        QSize size(mView->gridSize().width(), mView->iconSize().height());
        QPixmap pixmap = opt.icon.pixmap(mView->iconSize());
        opt.icon = QIcon(pixmap.copy(QRect(QPoint(0, 0), size)));
        opt.decorationSize = size;

        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
    }

private:
    QCategorizedView *mView;
};


RazorConfig::MainWindow::MainWindow() : QMainWindow()
{
    setupUi(this);

    model = new ConfigPaneModel();

    view->setViewMode(QListView::IconMode);
    view->setIconSize(QSize(32, 32));
    view->setGridSize(QSize(100, 100));
    view->setWordWrap(true);
    view->setUniformItemSizes(true);
    view->setCategoryDrawer(new QCategoryDrawerV3(view));

    connect(view, SIGNAL(activated(const QModelIndex&)),
            this, SLOT(activateItem(const QModelIndex&)));

    QTimer::singleShot(1, this, SLOT(load()));
}

void RazorConfig::MainWindow::load()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    proxyModel = new QCategorizedSortFilterProxyModel();
    proxyModel->setCategorizedModel(true);
    proxyModel->setSourceModel(model);

    view->setModel(proxyModel);
    view->setItemDelegate(new ConfigItemDelegate(view));

    QApplication::restoreOverrideCursor();
}

void RazorConfig::MainWindow::activateItem(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    QModelIndex orig = proxyModel->mapToSource(index);
    model->activateItem(orig);
}
