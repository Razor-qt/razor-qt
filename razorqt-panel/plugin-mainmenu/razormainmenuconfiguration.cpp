#include "razormainmenuconfiguration.h"
#include "ui_razormainmenuconfiguration.h"
#include "mainmenuselecticon.h"
#include "razorqt/xdgicon.h"

RazorMainMenuConfiguration::RazorMainMenuConfiguration(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorMainMenuConfiguration),
    mSettings(settings),
    mOldSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("MainMenuConfigurationWindow");
    ui->setupUi(this);

    connect(ui->selectIconTB, SIGNAL(clicked()), this, SLOT(showSelectIconWindow()));
    connect(ui->buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(dialogButtonsAction(QAbstractButton*)));
    connect(ui->showTextCB, SIGNAL(toggled(bool)), ui->textL, SLOT(setEnabled(bool)));
    connect(ui->showTextCB, SIGNAL(toggled(bool)), ui->textLE, SLOT(setEnabled(bool)));

    loadSettings();

    connect(ui->iconSizeSB, SIGNAL(valueChanged(int)), this, SLOT(saveSettings()));
    connect(ui->textLE, SIGNAL(textEdited(QString)), this, SLOT(saveSettings()));
    connect(ui->showTextCB, SIGNAL(toggled(bool)), this, SLOT(saveSettings()));
}

RazorMainMenuConfiguration::~RazorMainMenuConfiguration()
{
    delete ui;
}

void RazorMainMenuConfiguration::showSelectIconWindow()
{
   MainMenuSelectIcon *confWindow = this->findChild<MainMenuSelectIcon*>("MainMenuSelectIcon");

    if (!confWindow)
    {
        confWindow = new MainMenuSelectIcon(mSettings, this);
        connect(confWindow, SIGNAL(iconSelected()), this, SLOT(loadSettings()));
    }

    confWindow->show();
    confWindow->raise();
    confWindow->activateWindow();
}

void RazorMainMenuConfiguration::loadSettings()
{
    QString iconName = mSettings.value("button_icon", "").toString();
    QIcon icon;
    if (iconName[0] == '/')
    {
        icon.addFile(iconName);
    }
    else
    {
        icon = XdgIcon::fromTheme(iconName);
    }

    if (!icon.isNull() || !iconName.isEmpty())
    {
        ui->selectIconTB->setIcon(icon);
    }
    else
    {
        ui->selectIconTB->setStyleSheet("");
    }

    ui->iconSizeSB->setValue(mSettings.value("icon_size", 30).toInt());
    ui->showTextCB->setChecked(mSettings.value("showText", false).toBool());
    ui->textLE->setText(mSettings.value("text", "").toString());
}

void RazorMainMenuConfiguration::saveSettings()
{
    mSettings.setValue("button_icon", "");
    mSettings.setValue("icon_size", ui->iconSizeSB->value());
    mSettings.setValue("showText", ui->showTextCB->isChecked());
    mSettings.setValue("text", ui->textLE->text());
}

void RazorMainMenuConfiguration::dialogButtonsAction(QAbstractButton *btn)
{
    if (ui->buttons->buttonRole(btn) == QDialogButtonBox::ResetRole)
    {
        mOldSettings.loadToSettings();
        loadSettings();
    }
    else
    {
        close();
    }
}

