/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
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

#include <QtGui/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QProxyStyle>

#include <qtxdg/xdgicon.h>
#include <razorqt/razorsettings.h>
#include "mainwindow.h"


/*! Allow to handle item view/item widget itemActivated signals
for single or double click as it's specified in razor's configuration

\todo TODO/FIXME: maybe move it to libs (with some RazorApplication?)
*/
class RazorQProxyStyle : public QProxyStyle
{
public:
    RazorQProxyStyle()
    {
        RazorSettings s("desktop");
        m_singleClick = s.value("icon-launch-mode", "singleclick").toString() == "singleclick";
    }

    int styleHint(StyleHint hint, const QStyleOption *option = 0,
                  const QWidget *widget = 0, QStyleHintReturn *returnData = 0) const
    {
        if (hint == QStyle::SH_ItemView_ActivateItemOnSingleClick)
            return m_singleClick ? 1 : 0;

        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
private:
    bool m_singleClick;
};


int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    app.setStyle(new RazorQProxyStyle());
    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    app.setWindowIcon(QIcon(QString(SHARE_DIR) + "/graphics/razor_logo.png"));
    
    RazorConfig::MainWindow w;
    w.show();

    return app.exec();
}
