/********************************************************************
  Copyright: 2010 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*********************************************************************/

#ifndef XDGDESKTOPFILE_H
#define XDGDESKTOPFILE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QIcon>

class XdgDesktopFilePrivate;

/**
 \brief Desktop files handling.
 XdgDesktopFile class gives the interface for reading the values from the XDG .desktop file.
 The format .desktop a file is described on http://standards.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html

 The interface of this class is similar on QSettings.
 \author Petr Vanek <petr@scribus.info>
 */

class XdgDesktopFile : protected QObject
{
    Q_OBJECT
public:
    explicit XdgDesktopFile(QObject *parent = 0, const QString& prefix="Desktop Entry");
    explicit XdgDesktopFile(const QString& fileName, QObject *parent = 0, const QString& prefix="Desktop Entry");
    XdgDesktopFile(const XdgDesktopFile& other, QObject *parent = 0);
    virtual ~XdgDesktopFile();


    XdgDesktopFile& operator=(const XdgDesktopFile& other);

    /*! Returns the value for key. If the key doesn't exist, returns defaultValue.
        If no default value is specified, a default QVariant is returned. */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /*! Returns the localized value for key. In the desktop file keys may be postfixed by [LOCALE]. If the
        localized value doesn't exist, returns non lokalized value. If non localized value doesn't exist, returns defaultValue.
        LOCALE must be of the form lang_COUNTRY.ENCODING@MODIFIER, where _COUNTRY, .ENCODING, and @MODIFIER may be omitted.

        If no default value is specified, a default QVariant is returned. */
    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    //! Returns true if there exists a setting called key; returns false otherwise.
    bool contains(const QString& key) const;

    //! Returns true if the XdgDesktopFile is valid; otherwise returns false.
    virtual bool isValid() const;

    //! Returns the file name of the desktop file.
    QString fileName() const;

    /*! The desktop entry specification defines a number of fields to control the visibility of the application menu. This function
     checks whether to display a this application or not. */
    bool isShow(const QString& environment = "RAZOR") const;


    /*! A Exec value consists of an executable program optionally followed by one or more arguments.
        This function expands this arguments and returns command line string.
        @par urls - A list of files or URLS. Each file is passed as a separate argument to the result string program.*/
    QString expandExecString(const QStringList& urls = QStringList()) const;

    /*! Starts the program with the optional urls in a new process, and detaches from it. Returns true on success; otherwise returns false.
      @par urls - A list of files or URLS. Each file is passed as a separate argument to the executable program. */
    bool startDetached(const QStringList& urls) const;

    //! This function is provided for convenience. It's equivalent to calling startDetached(QStringList(url)).
    bool startDetached(const QString& url="") const;

    QIcon const icon(const QIcon& fallback = QIcon()) const;

    //! This function is provided for convenience. It's equivalent to calling localizedValue("Name").toString().
    QString name() const { return localizedValue("Name").toString(); }

    //! This function is provided for convenience. It's equivalent to calling localizedValue("Comment").toString().
    QString comment() const { return localizedValue("Comment").toString(); }

private:
    XdgDesktopFilePrivate* const d_ptr;
    Q_DECLARE_PRIVATE(XdgDesktopFile)
};

typedef QList<XdgDesktopFile*> XdgDesktopFileList;


class XdgDesktopFileCache
{
public:
    static XdgDesktopFile* getFile(const QString& fileName);
};


#endif // XDGDESKTOPFILE_H


