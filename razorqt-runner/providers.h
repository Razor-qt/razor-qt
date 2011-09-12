/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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


#ifndef PROVIDERS_H
#define PROVIDERS_H

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtXml/QDomElement>
#include <QtCore/QString>
#include <QtGui/QIcon>


class CommandProviderItem
{
public:
    CommandProviderItem();
    virtual ~CommandProviderItem();

    virtual bool run() const = 0;
    virtual bool compare(const QRegExp &regExp) const = 0;

    QIcon icon() const { return mIcon; }
    QString tile() const { return mTitle; }
    QString comment() const { return mComment; }
    QString toolTip() const { return mToolTip; }

protected:
    void setIcon(const QIcon &icon) { mIcon = icon; }
    void setTile(const QString &title);
    void setComment(const QString &comment);
    void setToolTip(const QString &toolTip) { mToolTip = toolTip; }


private:
    QIcon   mIcon;
    QString mTitle;
    QString mComment;
    QString mToolTip;
};



class CommandProvider: public QList<CommandProviderItem*>
{
public:
    CommandProvider();
    virtual ~CommandProvider();

    virtual void rebuild() {}
    virtual bool isOutDated() const { return false; }
};


/************************************************
 * Application desktop files
 ************************************************/

class AppLinkItem: public CommandProviderItem
{
public:
    AppLinkItem(const QDomElement &element);

    bool run() const;
    bool compare(const QRegExp &regExp) const;

private:
    QString mIcon;
    QString mSearchText;
    QString mDesktopFile;
};



class XdgMenu;
class AppLinkProvider: public CommandProvider
{
public:
    AppLinkProvider();
    virtual ~AppLinkProvider();

    void rebuild();
    bool isOutDated() const;

private:
    XdgMenu *mXdgMenu;
    void rebuildMainMenu(const QDomElement &xml);
};


/************************************************
 * History
 ************************************************/

class HistoryItem: public CommandProviderItem
{
public:
    HistoryItem(const QString &command);

    bool run() const;
    bool compare(const QRegExp &regExp) const;

    QString command() const { return mCommand; }

private:
    QString mCommand;
};



class QSettings;
class HistoryProvider: public CommandProvider
{
public:
    HistoryProvider();
    virtual ~HistoryProvider();

    void AddCommand(const QString &command);

private:
    QSettings *mHistoryFile;
};



/************************************************
 * Mathematics
 ************************************************/
class MathItem: public CommandProviderItem
{
public:
    MathItem();

    bool run() const;
    bool compare(const QRegExp &regExp) const;
};



class MathProvider: public CommandProvider
{
public:
    MathProvider();
    //virtual ~MathProvider();
};

#endif // PROVIDERS_H
