/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Marat "Morion" Talipov <morion.self@gmail.com>
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

#ifndef CONFIGPANELDIALOG_H
#define CONFIGPANELDIALOG_H

#include "../razorpanel.h"
#include <QSettings>
#include <QTimer>
#include <razorqt/razorconfigdialog.h>


class RazorPanel;

class ConfigPanelDialog : public RazorConfigDialog
{
    Q_OBJECT
public:
    static ConfigPanelDialog *exec(RazorPanel *panel);

    ConfigPanelDialog(RazorPanel *panel, QWidget *parent = 0);
};

namespace Ui {
class ConfigPanelWidget;
}

class ConfigPanelWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ConfigPanelWidget(RazorPanel *panel, QWidget *parent = 0);
    ~ConfigPanelWidget();

    int screenNum() const { return mScreenNum; }
    IRazorPanel::Position position() const { return mPosition; }


signals:
    void changed();

public slots:
    void reset();


private slots:
    void editChanged();
    void widthTypeChanged();
    void posittionChanged();

private:
    Ui::ConfigPanelWidget *ui;
    RazorPanel *mPanel;
    int mScreenNum;
    IRazorPanel::Position mPosition;

    void addPosition(const QString& name, int screen, RazorPanel::Position position);
    void fillComboBox_position();
    void fillComboBox_alignment();
    int indexForPosition(int screen, IRazorPanel::Position position);
    int getMaxLength();

    // For reset function
    int mOldLineSize;
    int mOldLineCount;
    int mOldLength;
    bool mOldLengthInPercents;
    RazorPanel::Alignment mOldAlignment;
    IRazorPanel::Position mOldPosition;
    int mOldScreenNum;
};

#endif // CONFIGPANELDIALOG_H
