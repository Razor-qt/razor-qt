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

#include "notepadwin.h"

#include <QtDebug>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>
#include <QFile>
#include <cmath>
#include <QTextEdit>
#include <iostream>
#include <QToolButton>
#include <qtxdg/xdgicon.h>

NotepadWin::NotepadWin(Notepad *notepad, SaveFunctionPointer sv, SaveFunctionPointer rpnt, QWidget *parent) : QWidget(parent)
{
    pad = notepad;
    saveText = sv;
    this->rpnt = rpnt;
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    edit = new QTextEdit();
    layout->addWidget(edit);
    connect(edit, SIGNAL(textChanged()), this, SLOT(textChanged()));
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
}

void NotepadWin::setParentSize(const QSizeF &size)
{
    qDebug() << "Notepad::setParentSize" << size;
    m_parentSize = size;
}

QString NotepadWin::text()
{
	return edit->toHtml();
}

void NotepadWin::setText(QString &text)
{
	edit->document()->setHtml(text);
}

void NotepadWin::textChanged()
{
	(pad->*saveText)();
}

void NotepadWin::setBold()
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    if(cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        bool isBold = false;
        int i = 0;
        for(i = selStart; i <= selEnd; i++) {
            cursor.setPosition(i);
            if(cursor.charFormat().fontWeight() == QFont::Bold) {
                isBold = true;
                break;
            }
        }
        cursor.setPosition(pos);
        fmt.setFontWeight(isBold ? QFont::Normal : QFont::Bold);
        cursor.mergeCharFormat(fmt);
        edit->mergeCurrentCharFormat(fmt);
    }
}

void NotepadWin::setItalic()
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    if(cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        bool isItalic = false;
        int i = 0;
        for(i = selStart; i <= selEnd; i++) {
            cursor.setPosition(i);
            if(cursor.charFormat().fontItalic()) {
                isItalic = true;
                break;
            }
        }
        cursor.setPosition(pos);
        fmt.setFontItalic(isItalic ? false : true);
        cursor.mergeCharFormat(fmt);
        edit->mergeCurrentCharFormat(fmt);
    }
}

void NotepadWin::setUnderline()
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    if(cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        bool isUnderline = false;
        int i = 0;
        for(i = selStart; i <= selEnd; i++) {
            cursor.setPosition(i);
            if(cursor.charFormat().fontUnderline()) {
                isUnderline = true;
                break;
            }
        }
        cursor.setPosition(pos);
        fmt.setFontUnderline(isUnderline ? false : true);
        cursor.mergeCharFormat(fmt);
        edit->mergeCurrentCharFormat(fmt);
    }
}

void NotepadWin::setStrike()
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    if(cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        bool isStrike = false;
        int i = 0;
        for(i = selStart; i <= selEnd; i++) {
            cursor.setPosition(i);
            if(cursor.charFormat().fontStrikeOut()) {
                isStrike = true;
                break;
            }
        }
        cursor.setPosition(pos);
        fmt.setFontStrikeOut(isStrike ? false : true);
        cursor.mergeCharFormat(fmt);
        edit->mergeCurrentCharFormat(fmt);
    }
}

void NotepadWin::setLeftSided()
{
    edit->setAlignment(Qt::AlignLeft);
    leftSided->setChecked(true);
    centered->setChecked(false);
    rightSided->setChecked(false);
    justified->setChecked(false);
}

void NotepadWin::setCentered()
{
    edit->setAlignment(Qt::AlignCenter);
    leftSided->setChecked(false);
    centered->setChecked(true);
    rightSided->setChecked(false);
    justified->setChecked(false);
}

void NotepadWin::setRightSided()
{
    edit->setAlignment(Qt::AlignRight);
    leftSided->setChecked(false);
    centered->setChecked(false);
    rightSided->setChecked(true);
    justified->setChecked(false);
}

void NotepadWin::setJustified()
{
    edit->setAlignment(Qt::AlignJustify);
    leftSided->setChecked(false);
    centered->setChecked(false);
    rightSided->setChecked(false);
    justified->setChecked(true);
}

void NotepadWin::onSelectionChanged()
{
    QTextCharFormat fmt;
    QTextCursor cursor = edit->textCursor();
    bool isBold = false;
    bool isItalic = false;
    bool isUnderline = false;
    bool isStrike = false;
    bool aLeft = false;
    bool aCenter = false;
    bool aRight = false;
    bool aJustify = false;
    if(cursor.hasSelection()) {
        int selStart = cursor.selectionStart();
        int selEnd = cursor.selectionEnd();
        int pos = cursor.position();
        int i = 0;

        for(i = selStart; i <= selEnd; i++) {
            cursor.setPosition(i);
            if(cursor.charFormat().fontWeight() == QFont::Bold)
                isBold = true;
            if(cursor.charFormat().fontItalic())
                isItalic = true;
            if(cursor.charFormat().fontUnderline())
                isUnderline = true;
            if(cursor.charFormat().fontStrikeOut())
                isStrike = true;
            if(edit->alignment() == Qt::AlignLeft)
                aLeft = true;
            else if(edit->alignment() == Qt::AlignCenter)
                aCenter = true;
            else if(edit->alignment() == Qt::AlignRight)
                aRight = true;
            else if(edit->alignment() == Qt::AlignJustify)
                aJustify = true;
        }
        cursor.setPosition(pos);

    }
    else {
        if(cursor.charFormat().fontWeight() == QFont::Bold)
            isBold = true;
        if(cursor.charFormat().fontItalic())
            isItalic = true;
        if(cursor.charFormat().fontUnderline())
            isUnderline = true;
        if(cursor.charFormat().fontStrikeOut())
            isStrike = true;
        if(edit->alignment() == Qt::AlignLeft)
            aLeft = true;
        else if(edit->alignment() == Qt::AlignCenter)
            aCenter = true;
        else if(edit->alignment() == Qt::AlignRight)
            aRight = true;
        else if(edit->alignment() == Qt::AlignJustify)
            aJustify = true;
    }
    bold->setChecked(isBold ? true : false);
    italic->setChecked(isItalic ? true : false);
    underline->setChecked(isUnderline ? true : false);
    strikethrough->setChecked(isStrike ? true : false);
    leftSided->setChecked(aLeft ? true : false);
    centered->setChecked(aCenter ? true : false);
    rightSided->setChecked(aRight ? true : false);
    justified->setChecked(aJustify ? true : false);
}
