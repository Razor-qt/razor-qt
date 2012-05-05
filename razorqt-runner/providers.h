/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#ifndef PROVIDERS_H
#define PROVIDERS_H

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtXml/QDomElement>
#include <QtCore/QString>
#include <QtGui/QIcon>

#define MAX_RANK 0xFFFF

/*! The CommandProviderItem class provides an item for use with CommandProvider.
    Items usually contain title, comment, toolTip and icon.
 */
class CommandProviderItem: public QObject
{
    Q_OBJECT
public:
    CommandProviderItem(): QObject() {}
    virtual ~CommandProviderItem() {}

    virtual bool run() const = 0;
    virtual bool compare(const QRegExp &regExp) const = 0;

    /// Returns the item's icon.
    QIcon icon() const { return mIcon; }

    /// Returns the item's title.
    QString title() const { return mTitle; }

    /// Returns the item's comment.
    QString comment() const { return mComment; }

    /// Returns the item's tooltip.
    QString toolTip() const { return mToolTip; }

    /*! The result of this function is used when searching for a apropriate item.
        The item with the highest rank will be highlighted.
            0 - not suitable.
            MAX_RANK - an exact match.
        In the most cases you can yse something like:
          return stringRank(mTitle, pattern);
     */
    virtual unsigned int rank(const QString &pattern) const  = 0;

protected:
    /// Helper function for the CommandProviderItem::rank
    unsigned int stringRank(const QString str, const QString pattern) const;
    QIcon   mIcon;
    QString mTitle;
    QString mComment;
    QString mToolTip;
};


/*! The CommandProvider class provides task for the razor-runner.
 */
class CommandProvider: public QObject, public QList<CommandProviderItem*>
{
    Q_OBJECT
public:
    CommandProvider();
    virtual ~CommandProvider();

    virtual void rebuild() {}
    virtual bool isOutDated() const { return false; }

signals:
    void aboutToBeChanged();
    void changed();
};


/************************************************
 * Application desktop files
 ************************************************/

class AppLinkItem: public CommandProviderItem
{
    Q_OBJECT
public:
    AppLinkItem(const QDomElement &element);

    bool run() const;
    bool compare(const QRegExp &regExp) const;
    QString command() const { return mCommand; }

    void operator=(const AppLinkItem &other);

    virtual unsigned int rank(const QString &pattern) const;
private slots:
    void updateIcon();
private:
    QString mDesktopFile;
    QString mIconName;
    QString mCommand;
    QString mProgram;
};


class XdgMenu;
class AppLinkProvider: public CommandProvider
{
    Q_OBJECT
public:
    AppLinkProvider();
    virtual ~AppLinkProvider();

private slots:
    void update();

private:
    XdgMenu *mXdgMenu;
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
    virtual unsigned int rank(const QString &pattern) const;

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
    void clearHistory();

private:
    QSettings *mHistoryFile;
};



/************************************************
 * Custom command
 ************************************************/
class CustomCommandProvider;

class CustomCommandItem: public CommandProviderItem
{
public:
    CustomCommandItem(CustomCommandProvider *provider);

    bool run() const;
    bool compare(const QRegExp &regExp) const;

    QString command() const { return mCommand; }
    void setCommand(const QString &command);

    virtual unsigned int rank(const QString &pattern) const;
private:
    QString mCommand;
    CustomCommandProvider *mProvider;
};



class QSettings;
class CustomCommandProvider: public CommandProvider
{
public:
    CustomCommandProvider();

    QString command() const { return mItem->command(); }
    void setCommand(const QString &command) { mItem->setCommand(command); }

    HistoryProvider* historyProvider() const { return mHistoryProvider; }
    void setHistoryProvider(HistoryProvider *historyProvider) { mHistoryProvider = historyProvider; }

private:
    CustomCommandItem *mItem;
    HistoryProvider *mHistoryProvider;
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
    virtual unsigned int rank(const QString &pattern) const;
};



class MathProvider: public CommandProvider
{
public:
    MathProvider();
    //virtual ~MathProvider();
};

#ifdef VBOX_ENABLED
#include <QtCore/QDateTime>
#include <QtGui/QDesktopServices>
#include <QtCore/QFileInfo>
#include <QtCore/QMap>
class VirtualBoxItem: public CommandProviderItem
{
public:
  VirtualBoxItem(const QString & MachineName , const QIcon & Icon);
  
  bool run() const;
  bool compare(const QRegExp &regExp) const;
  virtual unsigned int rank(const QString &pattern) const;
};

class VirtualBoxProvider: public CommandProvider
{
public:
  VirtualBoxProvider ();
  void rebuild();
  bool isOutDated() const;
  
private:
  QFile fp;
  QMap<QString,QString> osIcons;
  QString virtualBoxConfig;
  QDateTime timeStamp;
};
#endif


class QAction;
/*! Power management built in into runner
 */
class PowerProviderItem : public CommandProviderItem
{
public:
    PowerProviderItem(QAction *action);

    bool run() const;
    bool compare(const QRegExp &regExp) const;
    unsigned int rank(const QString &pattern) const;
private:
    QAction *m_action;
};

class PowerManager;
class ScreenSaver;
/*! Power management built in into runner
 */
class PowerProvider: public CommandProvider
{
    Q_OBJECT
public:
    PowerProvider();

private:
    PowerManager *m_power;
    ScreenSaver *m_screensaver;
};

#endif // PROVIDERS_H
