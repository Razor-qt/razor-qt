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
#include <QtCore/QTimer>

class Notepad;

typedef void (Notepad::*SaveFunctionPointer)(void);
typedef void (QTextCharFormat::*SetQTextCharFormat)(bool);
typedef bool (QTextCharFormat::*GetQTextCharFormat)(void) const;


struct NotepadFormat {
	bool bold;
	bool italic;
	bool underline;
	bool strike;
	bool left;
	bool center;
	bool right;
	bool justify;
};

class NotepadWin : public QWidget {
Q_OBJECT

public:
	NotepadWin(Notepad *notepad, SaveFunctionPointer sv, QWidget *parent = 0);

	QString text();
	int pos() const;
    void setParentSize(const QSizeF &size);
	void setTextAndPos(QString &text, int p);

protected:
	void showEvent(QShowEvent *event);

private:
	QSizeF m_parentSize;
	QTextEdit *edit;
	QVBoxLayout *layout;
	QHBoxLayout *panelLayout;
	SaveFunctionPointer saveText;
	Notepad *pad;
	QWidget *panel;
	QToolButton *bold, *italic, *underline, *strikethrough, *leftSided, *centered, *rightSided, *justified;
	QTimer* saveTimer;
	int scrollBarPosition;

	void updateFormat(const QTextCursor &cursor, NotepadFormat &format);
	void setFormat(bool bold = true, GetQTextCharFormat getter = NULL, SetQTextCharFormat setter = NULL);
	void setAlignment(Qt::AlignmentFlag a);

private slots:
    void save();

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
