/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;  only version 2 of
 * the License is valid for this program.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "commanditemmodel.h"

#include <razorqt/xdgmenu/xdgmenu.h>
#include <razorqt/xdgdesktopfile.h>
#include <razorqt/domhelper.h>
#include <razorqt/xdgicon.h>

#include <QtGui/QStandardItemModel>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QSettings>


/************************************************

 ************************************************/
CommandItem::CommandItem():
    QStandardItem()
{
}


/************************************************

 ************************************************/
CommandItem::~CommandItem()
{
}


/************************************************

 ************************************************/
void CommandItem::setText(const QString &title, const QString &comment)
{
    QString s = QString("<b>%1</b><br>\n%2\n").arg(title, comment);
    setData(s, Qt::DisplayRole);
}


/************************************************

 ************************************************/
DesktopItem::DesktopItem(const QDomElement &element):
    CommandItem()
{
    setData(XdgIcon::fromTheme(element.attribute("icon")), Qt::DecorationRole);
    setText(element.attribute("title"), element.attribute("genericName"));
    setData(QVariant(element.attribute("comment")), Qt::ToolTipRole);

    QString command = QFileInfo(element.attribute("exec")).baseName().section(" ", 0, 0);
    mSearchText = element.attribute("title") + " " + command;
    mDesktopFile = element.attribute("desktopFile");

}


/************************************************

 ************************************************/
bool DesktopItem::run() const
{

    XdgDesktopFile *desktop = XdgDesktopFileCache::getFile(mDesktopFile);
    return desktop->startDetached();
}


/************************************************

 ************************************************/
bool DesktopItem::compare(const QRegExp &regExp) const
{
    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseInsensitive);
    return mSearchText.contains(re);
}



/************************************************

 ************************************************/
HistoryItem::HistoryItem(const QString &command):
    CommandItem()
{
    setData(XdgIcon::defaultApplicationIcon(), Qt::DecorationRole);
    setText(command, QObject::tr("History"));
    mCommand = command;
}


/************************************************

 ************************************************/
bool HistoryItem::run() const
{
    return QProcess::startDetached(mCommand);
}


/************************************************

 ************************************************/
bool HistoryItem::compare(const QRegExp &regExp) const
{
    QRegExp re(regExp);

    re.setCaseSensitivity(Qt::CaseSensitive);
    return mCommand.contains(re);
}


/************************************************

 ************************************************/
CommandItemModel::CommandItemModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    mSourceModel(new QStandardItemModel(this)),
    mXdgMenu(new XdgMenu())
{
    setSourceModel(mSourceModel);
//    rebuild();
}


/************************************************

 ************************************************/
CommandItemModel::~CommandItemModel()
{
}


/************************************************

 ************************************************/
bool CommandItemModel::isOutDated() const
{
    return mXdgMenu->isOutDated();
}


/************************************************

 ************************************************/
void CommandItemModel::rebuild()
{
    if (mXdgMenu->isOutDated())
    {
        mXdgMenu->read(XdgMenu::getMenuFileName());

        // Remove old DesktopItems. If anyone knows a better solution tell me.
        for (int i = mSourceModel->rowCount()-1; i>-1; --i)
        {
            DesktopItem *item = dynamic_cast<DesktopItem*>(mSourceModel->item(i, 0));
            if (item)
                mSourceModel->removeRow(i, QModelIndex());
        }


        rebuildMainMenu(mXdgMenu->xml().documentElement());
    }
}


/************************************************

 ************************************************/
void CommandItemModel::rebuildMainMenu(const QDomElement &xml)
{
    DomElementIterator it(xml, "");
    while(it.hasNext())
    {
        QDomElement e = it.next();

        // Build submenu ........................
        if (e.tagName() == "Menu")
            rebuildMainMenu(e);

        //Build application link ................
        else if (e.tagName() == "AppLink")
        {
            DesktopItem *item = new DesktopItem(e);
            mSourceModel->appendRow(item);
        }

    }

}


/************************************************

 ************************************************/
bool CommandItemModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QRegExp re(filterRegExp());

    if (re.isEmpty())
        return false;

    CommandItem *item = dynamic_cast<CommandItem*>(mSourceModel->item(sourceRow, 0));

    if (!item)
        return false;

    return item->compare(re);
}


/************************************************

 ************************************************/
const CommandItem *CommandItemModel::command(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    QModelIndex ind = mapToSource(index);
    if (!ind.isValid())
        return 0;

    CommandItem *item = dynamic_cast<CommandItem*>(mSourceModel->item(ind.row(), 0));
    return item;
}


/************************************************

 ************************************************/
void CommandItemModel::addHistoryCommand(const QString &command)
{
    HistoryItem *item = new HistoryItem(command);
    mSourceModel->appendRow(item);
}



/************************************************

 ************************************************/
void CommandItemModel::loadHistory(const QSettings *settings)
{
    int n=0;
    while (true)
    {
        n++;
        QString command = settings->value(QString("history/command%1").arg(n)).toString();
        if (command.isEmpty())
            break;

        HistoryItem *item = new HistoryItem(command);
        mSourceModel->appendRow(item);
    }
}


/************************************************

 ************************************************/
void CommandItemModel::saveHistory(QSettings *settings)
{
    int n=0;
    for (int i=0; i<mSourceModel->rowCount(); ++i)
    {
        HistoryItem *item = dynamic_cast<HistoryItem*>(mSourceModel->item(i, 0));
        if (item)
        {
            n++;
            settings->setValue(QString("history/command%1").arg(n), item->command());
        }
    }
}
