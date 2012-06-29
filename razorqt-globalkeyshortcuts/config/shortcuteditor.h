#ifndef SHORTCUTEDITOR_H
#define SHORTCUTEDITOR_H

#include <QtGui/QTreeWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QItemDelegate>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QCursor>
#include <QtGui/QPushButton>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QMouseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QAction>

#include <razorqt/razorsettings.h>
#include "../../libraries/razorqt/razorshortcutbutton.h"
#include <qtxdg/xdgicon.h>

#include "ui_shortcuteditor.h"

#include <QDebug>
class CommandFinder : public QWidget
{
    Q_OBJECT

public:
    explicit CommandFinder(QWidget *parent = 0);

    QString text() { return m_lineEdit->text(); }
    void setText(const QString &txt) { m_lineEdit->setText(txt); }

private slots:
    void setCommand();

private:
    QLineEdit *m_lineEdit;
    QPushButton *m_button;
};

///
class CommandFinderDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit CommandFinderDelegate(QObject *parent = 0) : QItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

private slots:

};

///
class ShortCutDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ShortCutDelegate(QObject *parent = 0) : QItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:

private slots:

};
///

class ShortcutEditor : public QWidget , public Ui::ShortcutConfigWindow
{
    Q_OBJECT

public:
    ShortcutEditor(QWidget *parent = 0);
    ~ShortcutEditor();

private:
    QMenu *mMenu;

    /// mapping between key sequence and the item holding it
    QMap<QString, QTreeWidgetItem *> mapping;
    /// load shortcut settings (auto filter duplicate)
    void loadSettings();

private slots:
    /// add new group
    void addGroup();
    /// add new shortcut
    void addEmpty();
    /// remove current shortcut
    void removeCurrent();
    /// reset changes
    void resetChanges();

    /// show context menu
    void showMenu();

    /// shortcut changed , remove old one
    void shortcutChanged(QTreeWidgetItem *item , int col);

    /// edit on single click
    void editItem(QTreeWidgetItem *item , int column);

protected:

};

#endif // SHORTCUTEDITOR_H
