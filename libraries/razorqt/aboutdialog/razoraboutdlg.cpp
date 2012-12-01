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

#ifndef RAZORABOUTDLG_CPP
#define RAZORABOUTDLG_CPP
#include "razoraboutdlg.h"
#include "ui_razoraboutdlg.h"
#include "razoraboutdlg_p.h"
#include "libtranslate.h"
#include "technicalinfo.h"
#include "translatorsinfo/translatorsinfo.h"
#include <QDebug>
#include <QtCore/QDate>
#include <QtGui/QClipboard>

RazorAboutDLGPrivate::RazorAboutDLGPrivate()
{
    libTranslate("librazorqt");
    setupUi(this);

    QString css="<style TYPE='text/css'> "
                    "body { font-family: sans-serif;} "
                    ".name { font-size: 16pt; } "
                    "a { white-space: nowrap ;} "
                    "h2 { font-size: 10pt;} "
                    "li { line-height: 120%;} "
                    ".techInfoKey { white-space: nowrap ; margin: 0 20px 0 16px; } "
                "</style>"
            ;

    iconLabel->setFixedSize(48, 48);
    iconLabel->setScaledContents(true);
    iconLabel->setPixmap(QPixmap(QString(SHARE_DIR) + "/graphics/razor_logo.png"));

    nameLabel->setText(css + titleText());

    aboutBrowser->setHtml(css + aboutText());
    aboutBrowser->viewport()->setAutoFillBackground(false);

    autorsBrowser->setHtml(css + authorsText());
    autorsBrowser->viewport()->setAutoFillBackground(false);

    thanksBrowser->setHtml(css + thanksText());
    thanksBrowser->viewport()->setAutoFillBackground(false);

    translationsBrowser->setHtml(css + translationsText());
    translationsBrowser->viewport()->setAutoFillBackground(false);

    TechnicalInfo info;
    techBrowser->setHtml(info.html());
    techBrowser->viewport()->setAutoFillBackground(false);

    connect(techCopyToClipboardButton, SIGNAL(clicked()), this, SLOT(copyToCliboardTechInfo()));
    this->setAttribute(Qt::WA_DeleteOnClose);
    show();

}

QString RazorAboutDLGPrivate::titleText() const
{
    return QString("<div class=name>%1</div><div class=ver>%2</div>").arg(
                "Razor-qt",
                tr("Version: %1").arg(RAZOR_VERSION));

}

QString RazorAboutDLGPrivate::aboutText() const
{
    return  QString("<br>%1<br><br><br>%2<hr>%3<p>%4").arg(
                tr("Advanced, easy-to-use, and fast desktop environment based on Qt technologies."),
                tr("Copyright: %1-%2 %3").arg("2010", QDate::currentDate().toString("yyyy"), "Razor team"),

                tr("Homepage: %1").arg("<a href='http://www.razor-qt.org'>www.razor-qt.org</a>"),
                tr("License: %1").arg("<a href='http://www.gnu.org/licenses/lgpl-2.1.html'>GNU Lesser General Public License version 2.1 or later</a> "
                                      "and partly under the "
                                      "<a href='http://www.gnu.org/licenses/gpl-2.0.html'>GNU General Public License version 2</a>"
                                      )
                );
}

QString RazorAboutDLGPrivate::authorsText() const
{
    return QString("%1<p>%2").arg(
                tr("Razor-qt is developed by the <a %1>Razor-qt Team and contributors</a> on Github.")
                    .arg(" href='https://github.com/Razor-qt/razor-qt/graphs/contributors'"),
                tr("If you are interested in working with our development team, <a %1>join us</a>.")
                    .arg(" href='https://github.com/Razor-qt/razor-qt'")
                );
}


QString RazorAboutDLGPrivate::thanksText() const
{
    return QString(
                "%1"
                "<ul>"
                "<li>Andy Fitzsimon (logo/icon)</li>"
                "<li>Eugene Pivnev (QtDesktop)</li>"
                "<li>Paul Gnther (for desktop backgrounds)</li>"
                "<li>Manuel Meier (for ideas)</li>"
                "<li>Alexander Zakher (for the name)</li>"
                "<li>Alexey Nosov (for the A-MeGo theme)</li>"
                "<li>and KDE (http://www.kde.org)</li>"
                ).arg(tr("Special thanks to:"));
}

QString RazorAboutDLGPrivate::translationsText() const
{
    TranslatorsInfo translatorsInfo;
    return QString("%1<p><ul>%2</ul>").arg(
                tr("Razor-qt is translated into many languages thanks to the work of the translation teams all over the world."),
                translatorsInfo.asHtml()
                );
}

RazorAboutDLG::RazorAboutDLG()
{
    d_ptr = new RazorAboutDLGPrivate();
}

void RazorAboutDLGPrivate::copyToCliboardTechInfo()
{
    TechnicalInfo info;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(info.text());
}


#endif
