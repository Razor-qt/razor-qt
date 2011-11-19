/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.ru>
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
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

#include "dialog.h"
#include "ui_dialog.h"
#include "widgets.h"
#include "commanditemmodel.h"
#include "configuredialog/configuredialog.h"

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>
#include <razorqxt/qxtglobalshortcut.h>


#include <QtCore/QDebug>
#include <QtGui/QCloseEvent>
#include <QtGui/QDesktopWidget>
#include <QtCore/QProcess>
#include <QtGui/QLineEdit>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include <QScrollBar>
// I hate a X11 heading files. As a result we have such nightmare.
QEvent::Type QEventKeyPress=QEvent::KeyPress;
#include <razorqt/xfitman.h>

/************************************************

 ************************************************/
Dialog::Dialog(QWidget *parent) :
    QDialog(parent, Qt::Dialog | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint),
    ui(new Ui::Dialog),
    mSettings(new RazorSettings("razor-runner", this)),
    mGlobalShortcut(new QxtGlobalShortcut(this)),
    mCommandItemModel(new CommandItemModel(this))
{
    ui->setupUi(this);
    setStyleSheet(razorTheme->qss("razor-runner/razor-runner"));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(hide()));

    connect(mSettings, SIGNAL(settigsChanged()), this, SLOT(applySettings()));


    ui->commandEd->installEventFilter(this);
    ui->commandEd->setInsertPolicy(QComboBox::NoInsert);
    ui->commandEd->setCompleter(0);

    connect(ui->commandEd, SIGNAL(textChanged(QString)), this, SLOT(setFilter(QString)));
    connect(ui->commandEd->lineEdit(), SIGNAL(returnPressed()), this, SLOT(runCommand()));


    ui->commandList->installEventFilter(this);
    ui->commandList->setModel(mCommandItemModel);
    ui->commandList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->commandList, SIGNAL(clicked(QModelIndex)), this, SLOT(runCommand()));
    setFilter("");

    ui->commandList->setItemDelegate(new HtmlDelegate(QSize(32, 32), ui->commandList));
    connect(mGlobalShortcut, SIGNAL(activated()), this, SLOT(showHide()));

    // Popup menu ...............................
    QAction *a = new QAction(XdgIcon::fromTheme("configure"), tr("Configure razor-runner"), this);
    connect(a, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
    addAction(a);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    // End of popup menu ........................

    applySettings();
    resize(mSettings->value("dialog/width", 400).toInt(), size().height());

    XdgIcon::setThemeName(RazorSettings::globalSettings()->value("icon_theme").toString());
    connect(RazorSettings::globalSettings(), SIGNAL(iconThemeChanged()), this, SLOT(update()));
}


/************************************************

 ************************************************/
Dialog::~Dialog()
{
    delete ui;
}


/************************************************

 ************************************************/
void Dialog::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


/************************************************

 ************************************************/
QSize Dialog::sizeHint() const
{
    QSize size=QDialog::sizeHint();
    size.setWidth(this->size().width());
    return size;
}


/************************************************

 ************************************************/
void Dialog::resizeEvent(QResizeEvent *event)
{
    mSettings->setValue("dialog/width", size().width());
}


/************************************************

 ************************************************/
bool Dialog::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEventKeyPress) // QEvent::KeyPress
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

        if (object == ui->commandEd)
            return editKeyPressEvent(keyEvent);

        if (object == ui->commandList)
            return listKeyPressEvent(keyEvent);
    }

    return QDialog::eventFilter(object, event);
}


/************************************************
 eventFilter for ui->commandEd
 ************************************************/
bool Dialog::editKeyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_PageUp:
        if (ui->commandEd->currentText().isEmpty() &&
            ui->commandList->isVisible() &&
            ui->commandList->currentIndex().row() == 0
           )
        {
            setFilter("", false);
            return true;
        }
        qApp->sendEvent(ui->commandList, event);
        return true;

    case Qt::Key_Down:
    case Qt::Key_PageDown:
        if (ui->commandEd->currentText().isEmpty() &&
            ui->commandList->isHidden()
           )
        {
            setFilter("", true);
            return true;
        }

        qApp->sendEvent(ui->commandList, event);
        return true;
    }

    return QDialog::eventFilter(ui->commandList, event);
}


/************************************************
 eventFilter for ui->commandList
 ************************************************/
bool Dialog::listKeyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
    case Qt::Key_PageUp:
        if (ui->commandList->currentIndex().row() == 0)
        {
            ui->commandEd->setFocus();
            return true;
        }
        break;

    case Qt::Key_Left:
    case Qt::Key_Right:
        ui->commandEd->setFocus();
        qApp->sendEvent(ui->commandEd, event);
        return true;

    default:
        // Alphabetical or number key ...........
        if (!event->text().isEmpty())
        {
            ui->commandEd->setFocus();
            qApp->sendEvent(ui->commandEd, event);
            return true;
        }
    }

    return QDialog::eventFilter(ui->commandEd, event);
}


/************************************************

 ************************************************/
void Dialog::showHide()
{
    if (isVisible() &&  isActiveWindow())
        hide();
    else
    {
        realign();
        show();
        QApplication::setActiveWindow(this);
        activateWindow();
    }
}


/************************************************

 ************************************************/
void Dialog::realign()
{
    QRect desktop;

    if (mMonitor) // Show on the specified monitor.
        desktop = xfitMan().availableGeometry(mMonitor-1);
    else         // Follow the mouse.
        desktop = xfitMan().availableGeometry(QCursor::pos());


    QRect rect = this->geometry();
    rect.moveCenter(desktop.center());

    if (mShowOnTop)
        rect.moveTop(desktop.top());
    else
        rect.moveTop(desktop.center().y() - ui->panel->sizeHint().height());

    setGeometry(rect);
}


/************************************************

 ************************************************/
void Dialog::applySettings()
{
    // Shortcut .................................
    QKeySequence shortcut = QKeySequence::fromString(mSettings->value("dialog/shortcut", "Alt+F2").toString());
    if (shortcut.isEmpty())
        shortcut = QKeySequence::fromString("Alt+F2");

    if (mGlobalShortcut->shortcut() != shortcut)
    {
        mGlobalShortcut->setShortcut(shortcut);
        qDebug() << tr("Press \"%1\" to see dialog.").arg(shortcut.toString());
    }

    mShowOnTop = mSettings->value("dialog/show_on_top", true).toBool();

    mMonitor = mSettings->value("dialog/monitor", 0).toInt();

    realign();
    mSettings->sync();
}


/************************************************

 ************************************************/
void Dialog::setFilter(const QString &text, bool onlyHistory)
{
    if (mCommandItemModel->isOutDated())
        mCommandItemModel->rebuild();

    mCommandItemModel->showOnlyHistory(onlyHistory);
    mCommandItemModel->setFilterWildcard(text);

    if (mCommandItemModel->rowCount())
    {
        mCommandItemModel->sort(0);
        ui->commandList->setCurrentIndex(mCommandItemModel->appropriateItem(text));
        ui->commandList->scrollTo(ui->commandList->currentIndex());
        ui->commandList->show();
    }
    else
    {
        ui->commandList->hide();
    }

    adjustSize();
}


/************************************************

 ************************************************/
void Dialog::runCommand()
{
    bool res =false;
    const CommandProviderItem *command = mCommandItemModel->command(ui->commandList->currentIndex());

    if (command)
    {
        res = command->run();
    }
    else
    {
        QString command = ui->commandEd->currentText();
        res = QProcess::startDetached(command);
        if (res)
        {
            mCommandItemModel->addHistoryCommand(command);
        }
    }

    if (res)
    {
        hide();
        ui->commandEd->clearEditText();
    }

}



/************************************************

 ************************************************/
void Dialog::showConfigDialog()
{
    ConfigureDialog::createAndShow(mSettings, this);
}
