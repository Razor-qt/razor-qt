/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012-2013 Razor team
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

#ifndef RAZORAPPLICATION_H
#define RAZORAPPLICATION_H

#include <QtGui/QApplication>
#include <QtGui/QProxyStyle>


/*! \brief Razor-qt wrapper around QApplication.
 * It loads various Razor-qt related stuff by default (window icon, icon theme...)
 *
 * \note This wrapper is intended to be used only inside Razor-qt project. Using it
 *       in external application will automatically require linking to various
 *       Razor-qt libraries.
 *
 */
class RazorApplication : public QApplication
{
    Q_OBJECT

public:
    /*! Construct a Razor-qt application object.
     * \param argc standard argc as in QApplication
     * \param argv standard argv as in QApplication
     * \param stylesheetKey optional QString. It can load QSS theme module as in \see RazorTheme::qss()
     */
    RazorApplication(int &argc, char **argv, const QString &stylesheetKey = QString());
    virtual ~RazorApplication() {};

};


/*! \brief Modify currently used Qt QStyle to follow user preferences.
 *
 * For example: it allows to handle item view/item widget itemActivated signals
 * for single or double click as it's specified in razor's configuration
 */
class RazorQProxyStyle : public QProxyStyle
{
public:
    RazorQProxyStyle(QStyle * style = 0);

    int styleHint(StyleHint hint, const QStyleOption *option = 0,
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const;

private:
    bool m_singleClick;
};


#endif
