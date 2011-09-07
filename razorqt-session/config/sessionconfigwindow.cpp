/* BEGIN_COMMON_COPYRIGHT_HEADER
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * https://sourceforge.net/projects/razor-qt/
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Petr Vanek <petr@scribus.info>
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
 
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtGui/QStringListModel>
#include <QtGui/QLineEdit>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>

#include <razorqt/razorsettings.h>
#include <qtxdg/xdgicon.h>

#include "sessionconfigwindow.h"


bool findExecFile(const QString &name)
{
    QFileInfo fi(name);
    if (!name.startsWith("/"))
        fi.setFile("/usr/bin/" + name);
    return fi.exists();
}
    
QStringList findExecFiles(const QStringList &names)
{
    QStringList ret;
    foreach(QString i, names)
    {
        if (findExecFile(i))
            ret.append(i);
    }
    return ret;
}


SessionConfigWindow::SessionConfigWindow()
    : QMainWindow()
{
    setupUi(this);

    action_Quit->setIcon(XdgIcon::fromTheme("application-exit"));
    action_Clear_changes->setIcon(XdgIcon::fromTheme("edit-undo"));

    // pages
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-display-color"), tr("Basic Settings"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-filetype-association"), tr("Default Applications"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-desktop-launch-feedback"), tr("Autostart"), listWidget);
    new QListWidgetItem(XdgIcon::fromTheme("preferences-system-session-services"), tr("Environment (Advanced)"), listWidget);
    listWidget->setCurrentRow(0);

    m_settings = new RazorSettings("session", this);
    
    // basic settings ****************************************************
    
    // window managers
    QStringList knownWMs;
    QString wm = m_settings->value("windowmanager", "openbox").toString();
    knownWMs << "openbox" << "kwin" << "windowmaker" << "e16"
             << "fvwm2";
    handleCfgComboBox(wmComboBox, knownWMs, wm);
    
    // modules
    QHash<QString,QCheckBox*> modules;
    modules["razor-panel"] = panelCheckBox;
    modules["razor-desktop"] = desktopCheckBox;
    modules["razor-runner"] = runnerCheckBox;
    modules["razor-appswitcher"] = appswitcherCheckBox;
    
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
    knownBrowsers << "firefox" << "arora" << "konqueror" << "opera";
    handleCfgComboBox(browserComboBox, knownBrowsers, browser);


    // 3rd party autostart ***********************************************
    int count = m_settings->beginReadArray("autostart");
    QString app;
    QStringList appList;
    for (int i=0; i < count; ++i)
    {
        m_settings->setArrayIndex(i);
        app = m_settings->value("exec").toString();
        if (!app.isEmpty())
            appList << app;
    }
    m_settings->endArray();

    m_autostartModel = new QStringListModel(autostartView);
    autostartView->setModel(m_autostartModel);
    m_autostartModel->setStringList(appList);


    // environment variables (advanced) **********************************
    m_settings->beginGroup("environment");
    QString value;
    foreach (QString i, m_settings->childKeys())
    {
        value = m_settings->value(i).toString();
        QTreeWidgetItem *item = new QTreeWidgetItem(envTreeWidget, QStringList() << i << value);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        envTreeWidget->addTopLevelItem(item);
    }
    m_settings->endGroup();
    
    
    // UI stuff
    connect(action_Quit, SIGNAL(triggered()), this, SLOT(close()));
    connect(action_About, SIGNAL(triggered()), this, SLOT(about()));
    //
    connect(findWmButton, SIGNAL(clicked()), this, SLOT(findWmButton_clicked()));
    //
    connect(terminalButton, SIGNAL(clicked()), this, SLOT(terminalButton_clicked()));
    connect(browserButton, SIGNAL(clicked()), this, SLOT(browserButton_clicked()));
    //
    connect(appAddButton, SIGNAL(clicked()), this, SLOT(appAddButton_clicked()));
    connect(appDeleteButton, SIGNAL(clicked()), this, SLOT(appDeleteButton_clicked()));
    //
    connect(envAddButton, SIGNAL(clicked()), this, SLOT(envAddButton_clicked()));
    connect(envDeleteButton, SIGNAL(clicked()), this, SLOT(envDeleteButton_clicked()));

}

void SessionConfigWindow::closeEvent(QCloseEvent * event)
{
    m_settings->clear();

    // basic settings ****************************************************
    
    m_settings->setValue("windowmanager", wmComboBox->currentText());

    m_settings->beginGroup("modules");
    m_settings->setValue("razor-panel", panelCheckBox->isChecked());
    m_settings->setValue("razor-desktop", desktopCheckBox->isChecked());
    m_settings->setValue("razor-runner", runnerCheckBox->isChecked());
    m_settings->setValue("razor-appswitcher", appswitcherCheckBox->isChecked());
    m_settings->endGroup();

    
    // default applications **********************************************
    // see environment section


    // 3rd party autostart ***********************************************
    m_settings->beginWriteArray("autostart");
    int ix = 0;
    foreach(QString i, m_autostartModel->stringList())
    {
        m_settings->setArrayIndex(ix);
        m_settings->setValue("exec", i);
        ++ix;
    }
    m_settings->endArray();


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
    
    
    QMessageBox::information(this, tr("Session Restart Required"),
                             tr("You need to restart desktop session (razor-session) "
                                "to reload settings. Use logout from the main menu."
                               )
                            );
}

void SessionConfigWindow::handleCfgComboBox(QComboBox * cb,
                                            const QStringList &availableValues,
                                            const QString &value
                                           )
{
    QStringList realValues = findExecFiles(availableValues);
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
}

void SessionConfigWindow::terminalButton_clicked()
{
    updateCfgComboBox(terminalComboBox, tr("Select a terminal emulator"));
}

void SessionConfigWindow::browserButton_clicked()
{
    updateCfgComboBox(browserComboBox, tr("Select a web browser"));
}

void SessionConfigWindow::appAddButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Select Application"), "/usr/bin/");
    if (fname.isEmpty())
        return;
    
    QFileInfo fi(fname);
    if (!fi.exists() || !fi.isExecutable())
        return;
    
    QStringList l = m_autostartModel->stringList();
    l.append(fname);
    l.removeDuplicates();
    m_autostartModel->setStringList(l);
}

void SessionConfigWindow::appDeleteButton_clicked()
{
    QStringList l = m_autostartModel->stringList();
    int ix = autostartView->currentIndex().row();
    l.removeAt(ix);
    m_autostartModel->setStringList(l);
}

void SessionConfigWindow::envAddButton_clicked()
{
    QTreeWidgetItem *item = new QTreeWidgetItem(envTreeWidget, QStringList() << "" << "");
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    envTreeWidget->addTopLevelItem(item);
    envTreeWidget->setCurrentItem(item);
}

void SessionConfigWindow::envDeleteButton_clicked()
{
    foreach (QTreeWidgetItem *i, envTreeWidget->selectedItems())
    {
        delete i;
    }
}

void SessionConfigWindow::about()
{
    QMessageBox::about(this,
                       tr("About Razor Session Config"),
                       tr("<h1>Razor Session Config</h1>"
                          "<p><a href=\"http://razor-qt.sf.net\">Part of the Razor Desktop</a></p>")
                      );
    
}
