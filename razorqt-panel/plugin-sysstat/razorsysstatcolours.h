/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Kuzma Shapran <kuzma.shapran@gmail.com>
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


#ifndef RAZORSYSSTATCOLOURS_HPP
#define RAZORSYSSTATCOLOURS_HPP

#include <QtGui/QDialog>

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtGui/QColor>


namespace Ui {
    class RazorSysStatColours;
}

class QSignalMapper;
class QAbstractButton;
class QPushButton;

class RazorSysStatColours : public QDialog
{
    Q_OBJECT

public:
    explicit RazorSysStatColours(QWidget *parent = NULL);
    ~RazorSysStatColours();

    typedef QMap<QString, QColor> Colours;

    void setColours(const Colours&);

    Colours colours(void) const;

    Colours defaultColours(void) const;

signals:
    void coloursChanged(void);

public slots:
    void on_buttons_clicked(QAbstractButton*);

    void selectColour(const QString &);

    void restoreDefaults(void);
    void reset(void);
    void apply(void);

private:
    Ui::RazorSysStatColours *ui;

    QSignalMapper *mSelectColourMapper;
    QMap<QString, QPushButton*> mShowColourMap;

    Colours mDefaultColours;
    Colours mInitialColours;
    Colours mColours;

    void applyColoursToButtons(void);
};

#endif // RAZORSYSSTATCOLOURS_HPP
