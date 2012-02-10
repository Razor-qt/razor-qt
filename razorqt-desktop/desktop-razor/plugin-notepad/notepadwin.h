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
 
#ifndef NOTEPADWIN_H
#define NOTEPADWIN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QToolButton>

class Notepad;

typedef void (Notepad::*SaveFunctionPointer)(void);

class NotepadWin : public QWidget {
Q_OBJECT
	QSizeF m_parentSize;
	QTextEdit *edit;
	QVBoxLayout *layout;
	QHBoxLayout *panelLayout;
	SaveFunctionPointer saveText;
	Notepad *pad;
	QWidget *panel;
        QToolButton *bold, *italic, *underline, *strikethrough, *leftSided, *centered, *rightSided, *justified;
public:
    NotepadWin(Notepad *notepad, SaveFunctionPointer sv, QWidget *parent = 0);

    void setParentSize(const QSizeF &size);
	QString text();
	void setText(QString &text);

private slots:
	void textChanged();
public slots:
        void onSelectionChanged();
        void setBold();
        void setItalic();
        void setUnderline();
        void setStrike();
        void setLeftSided();
        void setCentered();
        void setRightSided();
        void setJustified();
};

#endif
