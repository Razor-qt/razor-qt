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

#ifndef RAZORSETTINGS_H
#define RAZORSETTINGS_H

#include <QtCore/QSettings>

class QEvent;

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
signals:
    void settigsChanged();

protected:
    virtual bool event(QEvent *event);

private:
    //RazorSettings(const QString& organization, const QString& application=QString(), QObject* parent=0);
    //RazorSettings(Scope scope, const QString& organization, const QString& application=QString(), QObject* parent=0);
    //RazorSettings(Format format, Scope scope, const QString& organization, const QString& application=QString(), QObject* parent=0);
    //RazorSettings(const QString& fileName, Format format, QObject* parent=0);

    RazorSettingsPrivate* const d_ptr;
    Q_DECLARE_PRIVATE(RazorSettings)
};

#endif // RAZORSETTINGS_H
