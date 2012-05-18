/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtGui/QStringListModel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgdesktopfile.h>
#include <qtxdg/xdgdirs.h>
#include <qtxdg/xdgicon.h>

#include "sessionconfigwindow.h"
#include "autostartedit.h"
#include "../src/windowmanager.h"


SessionConfigWindow::SessionConfigWindow()
    : QMainWindow(),
      m_restart(false)
{
    setupUi(this);

    // pages
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-display-color"), tr("Basic Settings"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-filetype-association"), tr("Default Applications"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-launch-feedback"), tr("Autostart"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-system-session-services"), tr("Environment (Advanced)"), listWidget);
    listWidget->setCurrentRow(0);

    m_settings = new RazorSettings("session", this);
    m_cache = new RazorSettingsCache(m_settings);
    restoreSettings();

    // UI stuff
    connect(findWmButton, SIGNAL(clicked()), this, SLOT(findWmButton_clicked()));
    //
    connect(terminalButton, SIGNAL(clicked()), this, SLOT(terminalButton_clicked()));
    connect(browserButton, SIGNAL(clicked()), this, SLOT(browserButton_clicked()));
    //
    connect(envAddButton, SIGNAL(clicked()), this, SLOT(envAddButton_clicked()));
    connect(envDeleteButton, SIGNAL(clicked()), this, SLOT(envDeleteButton_clicked()));
    //
    connect(autoStartAddButton, SIGNAL(clicked()), SLOT(autoStartAddButton_clicked()));
    connect(autoStartEditButton, SIGNAL(clicked()), SLOT(autoStartEditButton_clicked()));
    connect(autoStartDeleteButton, SIGNAL(clicked()), SLOT(autoStartDeleteButton_clicked()));
    //
    connect(wmComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setRestart()));
    connect(wmComboBox, SIGNAL(editTextChanged(const QString &)), this, SLOT(setRestart()));
    connect(panelCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(desktopCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(runnerCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(appswitcherCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(policyKitCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(notificationdCheckBox, SIGNAL(clicked()), this, SLOT(setRestart()));
    connect(terminalComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setRestart()));
    connect(terminalComboBox, SIGNAL(editTextChanged(const QString &)), this, SLOT(setRestart()));
    connect(browserComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setRestart()));
    connect(browserComboBox, SIGNAL(editTextChanged(const QString &)), this, SLOT(setRestart()));

    connect(buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
}

void SessionConfigWindow::restoreSettings()
{
    // window managers
    QStringList knownWMs;
    foreach (WindowManager i, getWindowManagerList(true))
    {
        knownWMs << i.command;
    }

    QString wm = m_settings->value("windowmanager", "openbox").toString();
    handleCfgComboBox(wmComboBox, knownWMs, wm);

    // modules
    QHash<QString,QCheckBox*> modules;
    modules["razor-panel"] = panelCheckBox;
    modules["razor-desktop"] = desktopCheckBox;
    modules["razor-runner"] = runnerCheckBox;
    modules["razor-appswitcher"] = appswitcherCheckBox;
    modules["razor-policykit-agent"] = policyKitCheckBox;
    modules["razor-notificationd"] = notificationdCheckBox;
    modules["razor-globalaccel"] = globalaccelCheckBox;
    
    m_settings->beginGroup("modules");
    foreach(QString i, modules.keys())
    {
        bool enable = m_settings->value(i, true).toBool();
        modules[i]->setChecked(enable);
    }
    m_settings->endGroup();

    // default applications **********************************************
    m_settings->beginGroup("environment");
    QString terminal = m_settings->value("TERM").toString();
    QString browser = m_settings->value("BROWSER").toString();
    m_settings->endGroup();
    
    if (terminal.isEmpty()) terminal = qgetenv("TERM");
    if (browser.isEmpty()) browser = qgetenv("BROWSER");
        
    QStringList knownTerms;
    knownTerms << "qterminal" << "xterm" << "konsole" << "uterm";
    handleCfgComboBox(terminalComboBox, knownTerms, terminal);
    QStringList knownBrowsers;
    knownBrowsers << "firefox" << "qupzilla" << "arora" << "konqueror" << "opera";
    handleCfgComboBox(browserComboBox, knownBrowsers, browser);

    // XDG autostart *****************************************************
    QAbstractItemModel* oldModel = xdgAutoStartView->model();
    mXdgAutoStartModel = new AutoStartItemModel(xdgAutoStartView);
    xdgAutoStartView->setModel(mXdgAutoStartModel);
    delete oldModel;
    xdgAutoStartView->setExpanded(mXdgAutoStartModel->index(0, 0), true);
    xdgAutoStartView->setExpanded(mXdgAutoStartModel->index(1, 0), true);
    autoStart_updateButtons();
    connect(mXdgAutoStartModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)), SLOT(autoStart_updateButtons()));
    connect(xdgAutoStartView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(autoStart_selectionChanged(QModelIndex)));

    // environment variables (advanced) **********************************
    m_settings->beginGroup("environment");
    QString value;
    envTreeWidget->clear();
    foreach (QString i, m_settings->childKeys())
    {
        value = m_settings->value(i).toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(envTreeWidget, QStringList() << i << value);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        envTreeWidget->addTopLevelItem(item);
    }
    m_settings->endGroup();
}

SessionConfigWindow::~SessionConfigWindow()
{
    delete m_cache;
}

void SessionConfigWindow::closeEvent(QCloseEvent * event)
{
    m_settings->clear();

    // basic settings ****************************************************
    
    m_settings->setValue("windowmanager", wmComboBox->currentText());

    m_settings->beginGroup("modules");
    m_settings->setValue("razor-panel", panelCheckBox->isChecked());
    m_settings->setValue("razor-globalaccel", globalaccelCheckBox->isChecked());
    m_settings->setValue("razor-desktop", desktopCheckBox->isChecked());
    m_settings->setValue("razor-runner", runnerCheckBox->isChecked());
    m_settings->setValue("razor-appswitcher", appswitcherCheckBox->isChecked());
    m_settings->setValue("razor-policykit-agent", policyKitCheckBox->isChecked());
    m_settings->setValue("razor-notificationd", notificationdCheckBox->isChecked());
    m_settings->endGroup();

    
    // default applications **********************************************
    // see environment section

    // XDG Autostart *****************************************************
    mXdgAutoStartModel->writeChanges();

    // environment variables (advanced) **********************************
    m_settings->beginGroup("environment");
    m_settings->remove("");
    for(int i = 0; i < envTreeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *item = envTreeWidget->topLevelItem(i);
        m_settings->setValue(item->text(0), item->text(1));
    }
    // handle TERM/BROWSER from defautl apps section
    if (!terminalComboBox->currentText().isEmpty())
        m_settings->setValue("TERM", terminalComboBox->currentText());
    if (!browserComboBox->currentText().isEmpty())
        m_settings->setValue("BROWSER", browserComboBox->currentText());
    m_settings->endGroup();
    
    if (m_restart) {
        QMessageBox::information(this, tr("Session Restart Required"),
                                tr("You need to restart desktop session (razor-session) "
                                    "to reload settings. Use logout from the main menu."
                                    )
                                );
    }
}

void SessionConfigWindow::handleCfgComboBox(QComboBox * cb,
                                            const QStringList &availableValues,
                                            const QString &value
                                           )
{
    QStringList realValues;
    foreach (QString s, availableValues)
    {
        if (findProgram(s))
            realValues << s;
    }
    cb->clear();
    cb->addItems(realValues);

    int ix = cb->findText(value);
    if (ix == -1)
        cb->lineEdit()->setText(value);
    else
        cb->setCurrentIndex(ix);
}

void SessionConfigWindow::updateCfgComboBox(QComboBox * cb,
                                            const QString &prompt
                                           )
{
    QString fname = QFileDialog::getOpenFileName(this, prompt, "/usr/bin/");
    if (fname.isEmpty())
        return;
    
    QFileInfo fi(fname);
    if (!fi.exists() || !fi.isExecutable())
        return;
    
    cb->lineEdit()->setText(fname);
}

void SessionConfigWindow::findWmButton_clicked()
{
    updateCfgComboBox(wmComboBox, tr("Select a window manager"));
    m_restart = true;
}

void SessionConfigWindow::terminalButton_clicked()
{
    updateCfgComboBox(terminalComboBox, tr("Select a terminal emulator"));
    m_restart = true;
}

void SessionConfigWindow::browserButton_clicked()
{
    updateCfgComboBox(browserComboBox, tr("Select a web browser"));
    m_restart = true;
}

void SessionConfigWindow::autoStartAddButton_clicked()
{
    AutoStartEdit edit(QString(), QString(), this);
    bool success = false;
    while (!success && edit.exec() == QDialog::Accepted)
    {
        QModelIndex index = xdgAutoStartView->selectionModel()->currentIndex();
        XdgDesktopFile file(XdgDesktopFile::ApplicationType, edit.name(), edit.command());
        if (mXdgAutoStartModel->addEntry(index, file))
            success = true;
        else
            QMessageBox::critical(this, tr("Error"), tr("File '%1' already exists!").arg(file.fileName()));
    }
}

void SessionConfigWindow::autoStartEditButton_clicked()
{
    QModelIndex index = xdgAutoStartView->selectionModel()->currentIndex();
    XdgDesktopFile* file = mXdgAutoStartModel->desktopFile(index);
    if (!file)
        return;
    AutoStartEdit edit(file->name(), file->value("Exec").toString());
    if (edit.exec() == QDialog::Accepted)
    {
        file->setLocalizedValue("Name", edit.name());
        file->setValue("Exec", edit.command());
    }
    mXdgAutoStartModel->setData(index, 0, Qt::UserRole);
}

void SessionConfigWindow::autoStartDeleteButton_clicked()
{
    QModelIndex index = xdgAutoStartView->selectionModel()->currentIndex();
    mXdgAutoStartModel->removeRow(index.row(), index.parent());
}

void SessionConfigWindow::autoStart_selectionChanged(QModelIndex curr)
{
    AutoStartItemModel::ActiveButtons active = mXdgAutoStartModel->activeButtons(curr);
    autoStartAddButton->setEnabled(active & AutoStartItemModel::AddButton);
    autoStartEditButton->setEnabled(active & AutoStartItemModel::EditButton);
    autoStartDeleteButton->setEnabled(active & AutoStartItemModel::DeleteButton);
}

void SessionConfigWindow::autoStart_updateButtons()
{
    autoStart_selectionChanged(xdgAutoStartView->selectionModel()->currentIndex());
}

void SessionConfigWindow::envAddButton_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(envTreeWidget, QStringList() << "" << "");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    envTreeWidget->addTopLevelItem(item);
    envTreeWidget->setCurrentItem(item);
    m_restart = true;
}

void SessionConfigWindow::envDeleteButton_clicked()
{
    foreach (QTreeWidgetItem *i, envTreeWidget->selectedItems())
    {
        delete i;
    }
    m_restart = true;
}

void SessionConfigWindow::setRestart()
{
    m_restart = true;
}

void SessionConfigWindow::dialogButtonsAction(QAbstractButton *btn)
{
    if (buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        m_cache->loadToSettings();
        restoreSettings();
    }
    else
    {
        close();
    }
}
