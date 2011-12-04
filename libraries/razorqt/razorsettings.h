/* BEGIN_COMMON_COPYRIGHT_HEADER
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


#ifndef RAZORSETTINGS_H
#define RAZORSETTINGS_H

#include <QtCore/QObject>
#include <QtCore/QSettings>


class QEvent;
    

/*! Get the config rirectory. Usually "~/.razor/" but it can be changed
    configuration/compilation phase.
    \note Path includes end-slash!
*/
QString razorConfigDir();


class RazorSettingsPrivate;
class RazorSettings : public QSettings
{
    Q_OBJECT
public:

    /*! \brief Constructs a RazorSettings object for accessing settings of the module called module, and with parent parent.
    RazorSettings can be accessed using the standard interface provided by QSettings, but it also provides some convenience functions and signals.

    \param module a base name of the config file. It's a name without
            the extension. For example: if you want to open settings for
            panel create it as RazorSettings("panel").
            The function will create all parent directories necessary to create
            the file.
    \param parent It's no need to delete this class manually if it's set.
    */
    explicit RazorSettings(const QString& module, QObject* parent = 0);
    //explicit RazorSettings(QObject* parent=0);
    explicit RazorSettings(const QSettings* parentSettings, const QString& subGroup, QObject* parent=0);
    explicit RazorSettings(const QSettings& parentSettings, const QString& subGroup, QObject* parent=0);
    ~RazorSettings();

    static const RazorSettings *globalSettings();
signals:
    void settigsChanged();

protected:
    virtual bool event(QEvent *event);

private:
    Q_DISABLE_COPY(RazorSettings)

    RazorSettingsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorSettings)
};


class RazorThemePrivate;
class RazorTheme : public QObject
{
    Q_OBJECT
public:
    /// \brief Returns a pointer to the RazorTheme instance.
    static RazorTheme* instance();

    /*! \brief Return StyleSheet text (not file name, but real text) of the module called module.
    Paths in url() C/QSS functions are parsed to be real values for the theme,
    relative to full path */
    QString qss(const QString& module) const;

    /*! \brief A full path to image used as a wallpaper
     \param screen is an ID of the screen like in Qt. -1 means default (any) screen.
                   Any other value greater than -1 is the exact screen (in dualhead).
                  In themes the index starts from 1 (ix 1 means 1st screen).
     \retval QString a file name (including path).
    */
    QString desktopBackground(int screen=-1) const;

private:
    RazorTheme();
    RazorTheme(const RazorTheme &);
    RazorTheme& operator=(const RazorTheme &);
    ~RazorTheme();

    static RazorTheme* mInstance;
    RazorThemePrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorTheme)
};

/*!
A global pointer referring to the unique RazorTheme object.
It is equivalent to the pointer returned by the RazorTheme::instance() function.
Only one theme object can be created. !*/

#define razorTheme RazorTheme::instance()

class RazorSettingsCache
{
public:
    explicit RazorSettingsCache(QSettings &settings);
    explicit RazorSettingsCache(QSettings *settings);
    virtual ~RazorSettingsCache() {}

    void loadFromSettings();
    void loadToSettings();

private:
    QSettings &mSettings;
    QHash<QString, QVariant> mCache;
};

class GlobalRazorSettingsPrivate;

class GlobalRazorSettings: public RazorSettings
{
    Q_OBJECT
public:
    GlobalRazorSettings();
    ~GlobalRazorSettings();

signals:
    /// Signal emitted when the icon theme has changed.
    void iconThemeChanged();

    /// Signal emitted when the razor theme has changed.
    void razorThemeChanged();

private slots:
    void fileChanged();

private:
    GlobalRazorSettingsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(GlobalRazorSettings)
};

#endif // RAZORSETTINGS_H
