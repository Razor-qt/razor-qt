/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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



#ifndef QTXDG_XDGDESKTOPFILE_H
#define QTXDG_XDGDESKTOPFILE_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QStringList>
#include <QIcon>

class XdgDesktopFilePrivate;

/**
 \brief Desktop files handling.
 XdgDesktopFile class gives the interface for reading the values from the XDG .desktop file.
 The interface of this class is similar on QSettings.

 The Desktop Entry Specification defines 3 types of desktop entries: Application, Link and
 Directory. The format of .desktop file is described on
 http://standards.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html

 Note that not all methods in this class make sense for all types of desktop files.
 \author Alexander Sokoloff <sokoloff.a@gmail.ru>
 */

class XdgDesktopFile : protected QObject
{
    Q_OBJECT
public:

    /*! The Desktop Entry Specification defines 3 types of desktop entries: Application, Link and
        Directory. File type is determined by the "Type" tag.
     */
    enum Type
    {
        UnknownType,     //! Unknown desktop file type. Maybe is invalid.
        ApplicationType, //! The file describes application.
        LinkType,        //! The file describes URL.
        DirectoryType    //! The file describes directory settings.
    };

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


    /*! Returns the desktop file type.
       @see XdgDesktopFile::Type
     */
    Type type() const;

    /*! A Exec value consists of an executable program optionally followed by one or more arguments.
        This function expands this arguments and returns command line string parts.
        Note this method make sense only for Application type.
        @par urls - A list of files or URLS. Each file is passed as a separate argument to the result string program.*/
    QStringList expandExecString(const QStringList& urls = QStringList()) const;

    /*! Returns the URL for the Link desktop file; otherwise an empty string is returned.
     */
    QString url() const;

    /*! For file with Application type. Starts the program with the optional urls in a new process, and detaches from it.
        Returns true on success; otherwise returns false.
          @par urls - A list of files or URLS. Each file is passed as a separate argument to the executable program.

        For file with Link type. Opens URL in the associated application. Parametr urls is not used.

        For file with Directory type, do nothing.  */
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
    static XdgDesktopFile* getDefaultApp(const QString& mimeType);
};


#endif // QTXDG_XDGDESKTOPFILE_H


