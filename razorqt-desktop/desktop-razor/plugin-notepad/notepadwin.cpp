/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2012 A-MeGo
 * http://a-mego.ru
 * Authors:
 *   Alexey Nosov <nay@a-mego.ru>
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

#include <QtCore/QtDebug>
#include <QtGui/QScrollBar>
#include <QtGui/QTextCharFormat>
#include <qtxdg/xdgicon.h>

#include "notepadwin.h"


NotepadWin::NotepadWin(Notepad *notepad, SaveFunctionPointer sv, QWidget *parent) :
    QWidget(parent),
    saveText(sv),
    pad(notepad),
    scrollBarPosition(0)
{
    saveTimer = new QTimer(this);
    saveTimer->setSingleShot(true);
    saveTimer->setInterval(1000);
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    edit = new QTextEdit();
    layout->addWidget(edit);

    panel = new QWidget();
    panel->setFixedHeight(25);
    layout->addWidget(edit);
    panelLayout = new QHBoxLayout();
    layout->addLayout(panelLayout);

    bold = new QToolButton();
    bold->setText(QString("B"));
    bold->setIcon(XdgIcon::fromTheme("format-text-bold"));

    italic = new QToolButton();
    italic->setText(QString("I"));
    italic->setIcon(XdgIcon::fromTheme("format-text-italic"));

    underline = new QToolButton();
    underline->setText(QString("U"));
    underline->setIcon(XdgIcon::fromTheme("format-text-underline"));

    strikethrough = new QToolButton();
    strikethrough->setText(QString("S"));
    strikethrough->setIcon(XdgIcon::fromTheme("format-text-strikethrough"));


    leftSided = new QToolButton();
    leftSided->setText(QString("L"));
    leftSided->setIcon(XdgIcon::fromTheme("format-justify-left"));


    centered = new QToolButton();
    centered->setText(QString("C"));
    centered->setIcon(XdgIcon::fromTheme("format-justify-center"));

    rightSided = new QToolButton();
    rightSided->setText(QString("R"));
    rightSided->setIcon(XdgIcon::fromTheme("format-justify-right"));

    justified = new QToolButton();
    justified->setText(QString("J"));
    justified->setIcon(XdgIcon::fromTheme("format-justify-fill"));


    int btnWidth = 21;
    bold->setCheckable(true);
    italic->setCheckable(true);
    underline->setCheckable(true);
    strikethrough->setCheckable(true);
    leftSided->setCheckable(true);
    centered->setCheckable(true);
    rightSided->setCheckable(true);
    justified->setCheckable(true);
    bold->setFixedWidth(btnWidth);
    italic->setFixedWidth(btnWidth);
    underline->setFixedWidth(btnWidth);
    strikethrough->setFixedWidth(btnWidth);
    leftSided->setFixedWidth(btnWidth);
    centered->setFixedWidth(btnWidth);
    rightSided->setFixedWidth(btnWidth);
    justified->setFixedWidth(btnWidth);
    bold->setObjectName(QString("boldButton"));
    italic->setObjectName(QString("italicButton"));
    underline->setObjectName(QString("underlineButton"));
    strikethrough->setObjectName(QString("strikethroughButton"));
    leftSided->setObjectName(QString("leftSided"));
    centered->setObjectName(QString("centered"));
    rightSided->setObjectName(QString("rightSided"));
    justified->setObjectName(QString("justified"));
    setObjectName(QString("notepadWin"));

    panelLayout->addWidget(bold);
    panelLayout->addWidget(italic);
    panelLayout->addWidget(underline);
    panelLayout->addWidget(strikethrough);

    panelLayout->setSpacing(0);
    panelLayout->addWidget(leftSided);
    panelLayout->addWidget(centered);
    panelLayout->addWidget(rightSided);
    panelLayout->addWidget(justified);
    layout->setAlignment(panelLayout, Qt::AlignLeft);

    connect(bold, SIGNAL(clicked()), this, SLOT(setBold()));
    connect(italic, SIGNAL(clicked()), this, SLOT(setItalic()));
    connect(underline, SIGNAL(clicked()), this, SLOT(setUnderline()));
    connect(strikethrough, SIGNAL(clicked()), this, SLOT(setStrike()));

    connect(leftSided, SIGNAL(clicked()), this, SLOT(setLeftSided()));
    connect(centered, SIGNAL(clicked()), this, SLOT(setCentered()));
    connect(rightSided, SIGNAL(clicked()), this, SLOT(setRightSided()));
    connect(justified, SIGNAL(clicked()), this, SLOT(setJustified()));

    connect(edit, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(edit, SIGNAL(cursorPositionChanged()), this, SLOT(onSelectionChanged()));

    connect(edit, SIGNAL(textChanged()), saveTimer, SLOT(start()));
    connect(edit->verticalScrollBar(), SIGNAL(valueChanged(int)), saveTimer, SLOT(start()));
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(save()));
}

QString NotepadWin::text()
{
    return edit->toHtml();
}

int NotepadWin::pos() const
{
    return edit->verticalScrollBar()->value();
}

void NotepadWin::setTextAndPos(QString &text, int p)
{
    edit->document()->setHtml(text);
    scrollBarPosition = p;
}

void NotepadWin::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    edit->verticalScrollBar()->setValue(scrollBarPosition);
}

void NotepadWin::updateFormat(const QTextCursor &cursor, NotepadFormat &format)
{
    format.bold = cursor.charFormat().fontWeight() == QFont::Bold ? true : false;
    format.italic = cursor.charFormat().fontItalic();
    format.underline = cursor.charFormat().fontUnderline();
    format.strike = cursor.charFormat().fontStrikeOut();
    format.left = format.right = format.center = format.justify = false;
    if(edit->alignment() == Qt::AlignLeft)
        format.left = true;
    else if(edit->alignment() == Qt::AlignCenter)
        format.center = true;
    else if(edit->alignment() == Qt::AlignRight)
        format.right = true;
    else if(edit->alignment() == Qt::AlignJustify)
        format.justify = true;
}

void NotepadWin::setFormat(bool bold, GetQTextCharFormat getter, SetQTextCharFormat setter)
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    if(cursor.hasSelection())
    {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        bool flag = false;
        int i = 0;
        for(i = selStart; i <= selEnd; i++)
        {
            cursor.setPosition(i);
            if(bold)
            {
                if(cursor.charFormat().fontWeight() == QFont::Bold)
                {
                    flag = true;
                    break;
                }
            }
            else if((cursor.charFormat().*getter)())
            {
                flag = true;
                break;
            }
        }
        cursor.setPosition(pos);
        if(bold)
            fmt.setFontWeight(flag ? QFont::Normal : QFont::Bold);
        else
            (fmt.*setter)(!flag);
        cursor.mergeCharFormat(fmt);
        edit->mergeCurrentCharFormat(fmt);
    }
}

void NotepadWin::setAlignment(Qt::AlignmentFlag a)
{
    edit->setAlignment(a);
    leftSided->setChecked(Qt::AlignLeft == a ? true : false);
    centered->setChecked(Qt::AlignCenter == a ? true : false);
    rightSided->setChecked(Qt::AlignRight == a ? true : false);
    justified->setChecked(Qt::AlignJustify == a ? true : false);
}

void NotepadWin::save()
{
    (pad->*saveText)();
}

void NotepadWin::setBold()
{
    setFormat();
}

void NotepadWin::setItalic()
{
    setFormat(false, &QTextCharFormat::fontItalic, &QTextCharFormat::setFontItalic);
}

void NotepadWin::setUnderline()
{
    setFormat(false, &QTextCharFormat::fontUnderline, &QTextCharFormat::setFontUnderline);
}

void NotepadWin::setStrike()
{
    setFormat(false, &QTextCharFormat::fontStrikeOut, &QTextCharFormat::setFontStrikeOut);
}

void NotepadWin::setLeftSided()
{
    setAlignment(Qt::AlignLeft);
}

void NotepadWin::setCentered()
{
    setAlignment(Qt::AlignCenter);
}

void NotepadWin::setRightSided()
{
    setAlignment(Qt::AlignRight);
}

void NotepadWin::setJustified()
{
    setAlignment(Qt::AlignJustify);
}

void NotepadWin::onSelectionChanged()
{
    QTextCursor cursor = edit->textCursor();
    NotepadFormat format;

    if(cursor.hasSelection())
    {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        int i = 0;

        for(i = selStart; i <= selEnd; i++)
        {
            cursor.setPosition(i);
            updateFormat(cursor, format);
        }
        cursor.setPosition(pos);

    }
    else
        updateFormat(cursor, format);
    bold->setChecked(format.bold);
    italic->setChecked(format.italic);
    underline->setChecked(format.underline);
    strikethrough->setChecked(format.strike);
    leftSided->setChecked(format.left);
    centered->setChecked(format.center);
    rightSided->setChecked(format.right);
    justified->setChecked(format.justify);
}
