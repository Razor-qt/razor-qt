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

#ifndef COMMANDITEMMODEL_H
#define COMMANDITEMMODEL_H

#include <QtGui/QSortFilterProxyModel>
#include <QtXml/QDomElement>
#include <QtCore/QStringList>
#include <QtGui/QStandardItem>

class QStandardItemModel;
class XdgMenu;
class QSettings;

class CommandItem: public QStandardItem
{
public:
    CommandItem();
    virtual ~CommandItem();

    virtual bool run() const = 0;
    virtual bool compare(const QRegExp &regExp) const = 0;

protected:
    void setText(const QString &title, const QString &comment);
};


class DesktopItem: public CommandItem
{
public:
    DesktopItem(const QDomElement &element);

    bool run() const;
    bool compare(const QRegExp &regExp) const;

private:
    QString mSearchText;
    QString mDesktopFile;
};


class HistoryItem: public CommandItem
{
public:
    HistoryItem(const QString &command);

    bool run() const;
    bool compare(const QRegExp &regExp) const;

    QString command() const { return mCommand; }

private:
    QString mCommand;
};


class CommandItemModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
//    enum DataRoles
//    {
//        CaseSensitiveSearchRole     = Qt::UserRole  + 1,
//        CaseInsensitiveSearchRole,
//        FileRole
//    };

    explicit CommandItemModel(QObject *parent = 0);
    virtual ~CommandItemModel();

    bool isOutDated() const;
    const CommandItem *command(const QModelIndex &index) const;

    void addHistoryCommand(const QString &command);
    void loadHistory(const QSettings *settings);
    void saveHistory(QSettings *settings);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;


public slots:
    void rebuild();

private:
    QStandardItemModel *mSourceModel;
    XdgMenu *mXdgMenu;
    void rebuildMainMenu(const QDomElement &xml);
};

#endif // COMMANDITEMMODEL_H

