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

#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class Dialog;
}

class RazorSettings;
class QxtGlobalShortcut;
class CommandListView;
class CommandItemModel;

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    QSize sizeHint() const;

protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
    bool eventFilter(QObject *object, QEvent *event);
    bool editKeyPressEvent(QKeyEvent *event);
    bool listKeyPressEvent(QKeyEvent *event);

private:
    Ui::Dialog *ui;
    RazorSettings *mSettings;
    QxtGlobalShortcut *mGlobalShortcut;
    CommandItemModel *mCommandItemModel;
    bool mShowOnTop;
    int mMonitor;

    void realign();
    //! \brief handle various additional behaviours (math only for now)
    bool editEnterPressed();

private slots:
    void applySettings();
    void showHide();
    void setFilter(const QString &text);
    void runCommand();
    void showConfigDialog();
};

#endif // DIALOG_H

