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
#include <QtGui/QStatusBar>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>

#include "mainwindow.h"
#include <QtDebug>

#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgicon.h>
#include <razorqt/razoraboutdlg.h>


namespace RazorConfig {

class ConfigItem : public QListWidgetItem
{
    XdgDesktopFile *m_xdg;

public:
    ConfigItem(XdgDesktopFile *xdg, QListWidget *parent)
        : QListWidgetItem(parent),
          m_xdg(xdg)
    {
        setIcon(xdg->icon(XdgIcon::defaultApplicationIcon()));
        setText(xdg->name());
        setToolTip(xdg->comment().isEmpty() ? xdg->name() : xdg->comment());
    }
    ConfigItem(QListWidget *parent)
        : QListWidgetItem(parent),
          m_xdg(0)
    {
    }

    void start()
    {
        if (m_xdg)
            m_xdg->startDetached();
    }
};

class TitleItem : public ConfigItem
{
public:
    TitleItem(const QString &title, QListWidget *parent)
        : ConfigItem(parent)
    {
        setText(title);
        setFlags(Qt::NoItemFlags); // disable the item completely
        
        QFont f(font());
        f.setBold(true);
        setFont(f);
        
        QBrush bg = background();
        setBackground(foreground());
        setForeground(background());
    }
    
    void start() {};
};

class ItemDelegate: public QStyledItemDelegate
{
    QListWidget* mView;

public:
    explicit ItemDelegate(QListWidget *parent)
        : QStyledItemDelegate(parent),
          mView(parent)
    {
    }
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setWidth(mView->viewport()->width());
        size.setHeight(size.height()*3);
        return size;
    }
};

} // namespace RazorConfig


RazorConfig::MainWindow::MainWindow() : QMainWindow()
{
    setupUi(this);
    
    QDirIterator it("/usr/share/applications", QStringList() << "*.desktop", QDir::NoFilter, QDirIterator::Subdirectories);
    QString name;
    QString categories;
    
    QHash<QString,QList<XdgDesktopFile*> > map;
    // ensure that razor goes first
    map["Razor"] = QList<XdgDesktopFile*>();

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
        
        categories = xdg->value("Categories").toString();
        if (!categories.contains("Settings"))
        {
            delete xdg;
            continue;
        }
        
        if (categories.contains("X-RAZOR") || categories.contains("RAZOR"))
        {
            map["Razor"].append(xdg);
        }
        else if (categories.contains("System"))
        {
            map[tr("System")].append(xdg);
        }
        else
        {
            map[tr("Other")].append(xdg);
        }
    }

    foreach (QString title, map.keys())
    {
        new TitleItem(title, listWidget);
        listWidget->setItemDelegateForRow(listWidget->count()-1, new ItemDelegate(listWidget));
        foreach (XdgDesktopFile*i, map[title])
        {
            new ConfigItem(i, listWidget);
        }
    }
    
    connect(listWidget, SIGNAL(itemActivated(QListWidgetItem*)),
            this, SLOT(listWidget_itemDoubleClicked(QListWidgetItem *)));
}


void RazorConfig::MainWindow::listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // we know it's ConfigItem only.
    reinterpret_cast<ConfigItem*>(item)->start();
}
