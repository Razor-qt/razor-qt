/* coded by Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 * (c)DWTFYW
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */
#ifndef SELECTWND_H
#define SELECTWND_H

#include <QtCore/QObject>
#include <QtGui/QWidget>

class XCursorThemeModel;

#include "ui_selectwnd.h"
class SelectWnd : public QWidget, private Ui_SelectWnd
{
    Q_OBJECT

public:
    SelectWnd (QWidget *parent=0);
    ~SelectWnd ();

public slots:
    void setCurrent ();

protected:
    void keyPressEvent (QKeyEvent *e);

private:
    bool iconsIsWritable () const;
    void selectRow (int) const;
    void selectRow (const QModelIndex &index) const { selectRow(index.row()); }

private slots:
    void currentChanged (const QModelIndex &current, const QModelIndex &previous);
    void on_btInstall_clicked ();
    void on_btRemove_clicked ();
    void on_btSet_clicked ();

private:
    XCursorThemeModel *mModel;
    QPersistentModelIndex mAppliedIndex;
};

#endif
