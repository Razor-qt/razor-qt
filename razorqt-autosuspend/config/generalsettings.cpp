/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Razor - a lightweight, Qt based, desktop toolset
 * http://razor-qt.org
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Christian Surlykke <christian@surlykke.dk>
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
#include <QtGui/QAbstractButton>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnectionInterface>

#include "generalsettings.h"
#include "ui_generalsettings.h"

GeneralSettings::GeneralSettings(RazorSettings *settings, RazorConfigDialog *parent) :
    QWidget(parent),
    mUi(new Ui::GeneralSettings),
    mAutostart("razor-autosuspend.desktop"),
    mLoading(false),
    mModuleRunning(false)
{
    this->mSettings = settings;
    mUi->setupUi(this);

    connect(mUi->startupCheckBox, SIGNAL(toggled(bool)), this, SLOT(startModule(bool)));
    connect(mUi->showTrayIconCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
    connect(mUi->useThemeStatusIconsCheckBox, SIGNAL(stateChanged(int)), this, SLOT(saveSettings()));
    connect(parent, SIGNAL(save()), SLOT(onClose()));
}

GeneralSettings::~GeneralSettings()
{
    delete mUi;
}

void GeneralSettings::saveSettings()
{
    if (mLoading) return; // If we come heare because checkboxes changed state during loading
                         // we don't wan't to save

    mSettings->setValue(SHOWTRAYICON_KEY, mUi->showTrayIconCheckBox->isChecked());
    mSettings->setValue(USETHEMEICONS_KEY, mUi->useThemeStatusIconsCheckBox->isChecked());
    mUi->useThemeStatusIconsCheckBox->setEnabled(mUi->showTrayIconCheckBox->isChecked());
}

void GeneralSettings::loadSettings()
{
    mLoading = true;
    mAutostart = RazorAutostartEntry("razor-autosuspend.desktop");
    mUi->startupCheckBox->setChecked(mAutostart.isEnabled());
    mUi->showTrayIconCheckBox->setChecked(mSettings->value(SHOWTRAYICON_KEY, true).toBool());
    mUi->useThemeStatusIconsCheckBox->setChecked(mSettings->value(USETHEMEICONS_KEY, true).toBool());
    mUi->useThemeStatusIconsCheckBox->setEnabled(mUi->showTrayIconCheckBox->isChecked());
    mLoading = false;
}

void GeneralSettings::startModule(bool enable)
{
    mAutostart.setEnabled(enable);

    if (!enable || mModuleRunning)
        return;

    QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
    QStringList services = reply.value();
    if (!services.contains("org.razorqt.razor-autosuspend"))
    {
        if (services.contains("org.razorqt.session"))
        {
            QDBusInterface interface("org.razorqt.session", "/RazorSession", "",
                                     QDBusConnection::sessionBus(), this);
            QList<QVariant> arg;
            arg.append(mAutostart.name());
            interface.callWithArgumentList(QDBus::NoBlock, "startModule", arg);
        }
        else // in case razor-session is not being used
        {
            mAutostart.file().startDetached();
        }
    }
    mModuleRunning = true;
}

void GeneralSettings::onClose()
{
    if (!mAutostart.isEnabled())
    {
        QDBusInterface interface("org.razorqt.session", "/RazorSession", "",
                                 QDBusConnection::sessionBus(), this);
        QList<QVariant> arg;
        arg.append(mAutostart.name());
        interface.callWithArgumentList(QDBus::NoBlock, "stopModule", arg);
    }
    mAutostart.commit();
}
