/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL3+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
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

#include <QtCore/QSharedDataPointer>
//#include <QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QStringList>
#include <QtGui/QIcon>

class XdgDesktopFilePrivate;
class XdgDesktopFileData;
	
/**
 \brief Desktop files handling.
 XdgDesktopFile class gives the interface for reading the values from the XDG .desktop file.
 The interface of this class is similar on QSettings. XdgDesktopFile objects can be passed
 around by value since the XdgDesktopFile class uses implicit data sharing.

 The Desktop Entry Specification defines 3 types of desktop entries: Application, Link and
 Directory. The format of .desktop file is described on
 http://standards.freedesktop.org/desktop-entry-spec/desktop-entry-spec-latest.html

 Note that not all methods in this class make sense for all types of desktop files.
 \author Alexander Sokoloff <sokoloff.a@gmail.ru>
 */

class XdgDesktopFile
{
public:
    /*! The Desktop Entry Specification defines 3 types of desktop entries: Application, Link and
        Directory. File type is determined by the "Type" tag. */
    enum Type
    {
        UnknownType,     //! Unknown desktop file type. Maybe is invalid.
        ApplicationType, //! The file describes application.
        LinkType,        //! The file describes URL.
        DirectoryType    //! The file describes directory settings.
    };

    //! Constructs an empty XdgDesktopFile
    XdgDesktopFile();

    /*! Constructs a copy of other.
        This operation takes constant time, because XdgDesktopFile is implicitly shared. This makes
        returning a XdgDesktopFile from a function very fast. If a shared instance is modified,
        it will be copied (copy-on-write), and that takes linear time. */
    XdgDesktopFile(const XdgDesktopFile& other);

    //! Destroys the object.
    virtual ~XdgDesktopFile();

    //! Assigns other to this DesktopFile and returns a reference to this DesktopFile.
    XdgDesktopFile& operator=(const XdgDesktopFile& other);

    //! Loads an DesktopFile from the file with the given fileName.
    virtual bool load(const QString& fileName);

    //! Saves the DesktopFile to the file with the given fileName. Returns true if successful; otherwise returns false.
    virtual bool save(const QString &fileName) const;


    /*! This is an overloaded function.
        This function writes a DesktopFile to the given device. */
    virtual bool save(QIODevice *device) const;

    /*! Returns the value for key. If the key doesn't exist, returns defaultValue.
        If no default value is specified, a default QVariant is returned. */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /*! Returns the localized value for key. In the desktop file keys may be postfixed by [LOCALE]. If the
        localized value doesn't exist, returns non lokalized value. If non localized value doesn't exist, returns defaultValue.
        LOCALE must be of the form lang_COUNTRY.ENCODING@MODIFIER, where _COUNTRY, .ENCODING, and @MODIFIER may be omitted.

        If no default value is specified, a default QVariant is returned. */
    QVariant localizedValue(const QString& key, const QVariant& defaultValue = QVariant()) const;

    //! Sets the value of setting key to value. If the key already exists, the previous value is overwritten.
    void setValue(const QString &key, const QVariant &value);

    //! Returns true if there exists a setting called key; returns false otherwise.
    bool contains(const QString& key) const;

    //! Returns true if the XdgDesktopFile is valid; otherwise returns false.
    bool isValid() const;

    //! Returns the file name of the desktop file.
    QString fileName() const;

    //! Returns an icon specified in this file.
    QIcon const icon(const QIcon& fallback = QIcon()) const;

    //! Returns an icon name specified in this file.
    QString const iconName() const;

    //! This function is provided for convenience. It's equivalent to calling localizedValue("Name").toString().
    QString name() const { return localizedValue("Name").toString(); }

    //! This function is provided for convenience. It's equivalent to calling localizedValue("Comment").toString().
    QString comment() const { return localizedValue("Comment").toString(); }

    /*! Returns the desktop file type.
        @see XdgDesktopFile::Type */
    Type type() const;

    /*! For file with Application type. Starts the program with the optional urls in a new process, and detaches from it.
        Returns true on success; otherwise returns false.
          @par urls - A list of files or URLS. Each file is passed as a separate argument to the executable program.

        For file with Link type. Opens URL in the associated application. Parametr urls is not used.

        For file with Directory type, do nothing.  */
    bool startDetached(const QStringList& urls) const;

    //! This function is provided for convenience. It's equivalent to calling startDetached(QStringList(url)).
    bool startDetached(const QString& url="") const;

    /*! A Exec value consists of an executable program optionally followed by one or more arguments.
        This function expands this arguments and returns command line string parts.
        Note this method make sense only for Application type.
        @par urls - A list of files or URLS. Each file is passed as a separate argument to the result string program.*/
    QStringList expandExecString(const QStringList& urls = QStringList()) const;

    /*! Returns the URL for the Link desktop file; otherwise an empty string is returned.  */
    QString url() const;

    /*! The desktop entry specification defines a number of fields to control the visibility of the application menu. This function
         checks whether to display a this application or not. */
    bool isShow(const QString& environment = "RAZOR") const;

    /*! This fuction returns true if the desktop file is applicable to the current environment.
        @par excludeHidden - if set to true (default), files with "Hidden=true" will be considered "not applicable".
                             Setting this to false is be useful when the user wants to enable/disable items and wants to see those
                             that are Hidden */
    bool isApplicable(bool excludeHidden = true, const QString& environment = "RAZOR") const;

protected:
    virtual QString prefix() const { return "Desktop Entry"; }
    virtual bool check() const { return true; }
private:
    QSharedDataPointer<XdgDesktopFileData> d;
};


typedef QList<XdgDesktopFile> XdgDesktopFileList;


class XdgDesktopFileCache
{
public:
    static XdgDesktopFile* getFile(const QString& fileName);
    static XdgDesktopFile* getDefaultApp(const QString& mimeType);
};


#endif // QTXDG_XDGDESKTOPFILE_H


