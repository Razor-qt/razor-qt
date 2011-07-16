#include "mainmenuselecticon.h"
#include "ui_mainmenuselecticon.h"
#include "razorqt/xdgicon.h"

#include <QtGui/QFileDialog>

MainMenuSelectIcon::MainMenuSelectIcon(QSettings &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainMenuSelectIcon),
    mSettings(settings)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setObjectName("MainMenuSelectIcon");
    ui->setupUi(this);

    connect(ui->iconTypeCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(iconTypeChanged(QString)));
    connect(ui->iconList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(saveIcon(QListWidgetItem*)));
    connect(ui->chooseIconPB, SIGNAL(clicked()), this, SLOT(chooseIconDialog()));
    connect(ui->systemIconRB, SIGNAL(toggled(bool)), this, SLOT(systemIconChecked(bool)));
    connect(ui->userIconRB, SIGNAL(toggled(bool)), this, SLOT(userIconChecked(bool)));

    mIconTheme = XdgIcon::themeName();

    if (findIconTypes() == 0)
    {
    }
}

MainMenuSelectIcon::~MainMenuSelectIcon()
{
    delete ui;
}

void MainMenuSelectIcon::systemIconChecked(bool state)
{
    if (state == true)
    {
        ui->iconList->clear();
        ui->iconTypeCB->setEnabled(true);
        iconTypeChanged(ui->iconTypeCB->currentText());
    }
    else
    {
        ui->iconTypeCB->setEnabled(false);
    }
}

void MainMenuSelectIcon::userIconChecked(bool state)
{
    if (state == true)
    {
        ui->iconList->clear();
        ui->chooseIconPB->setEnabled(true);
    }
    else
    {
        ui->chooseIconPB->setEnabled(false);
    }
}

int MainMenuSelectIcon::findIconTypes()
{
    QStringList iconPathes;
    iconPathes << "/usr/share/icons/" << "~/.icons";

    foreach (QString path, iconPathes)
    {
        mIconThemeDir.setPath(path + mIconTheme);
        if (mIconThemeDir.exists() == false)
        {
            mIconThemeDir.setPath("");
        }
        else
        {
            break;
        }
    }

    /* Maybe better idea is reading types from index.theme file */
    mIconThemeDir.setPath(mIconThemeDir.path() + "/32x32");
    ui->iconTypeCB->addItems(mIconThemeDir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot));

    return 1;
}

void MainMenuSelectIcon::iconTypeChanged(QString type)
{
    ui->iconList->clear();
    mIconThemeDir.cd(type);
    QString test = mIconThemeDir.path();
    QFileInfoList fileList = mIconThemeDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
    foreach (QFileInfo file, fileList)
    {
        ui->iconList->addItem(new QListWidgetItem(XdgIcon::fromTheme(file.baseName()), file.baseName()));
    }

    mIconThemeDir.cdUp();
}

void MainMenuSelectIcon::saveIcon(QListWidgetItem *item)
{
    if (ui->systemIconRB->isChecked() == true)
    {
        mSettings.setValue("button_icon", item->text());
    }

    if (ui->userIconRB->isChecked() == true)
    {
        mSettings.setValue("button_icon", mOwnIconsDir.path() + "/" + item->text());
    }

    emit iconSelected();
    this->close();
}

void MainMenuSelectIcon::chooseIconDialog()
{
    ui->iconList->clear();
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    if (fileDialog.exec())
    {
        mOwnIconsDir.setPath(fileDialog.selectedFiles()[0]);
        QFileInfoList fileList = mOwnIconsDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
        QIcon *icon;
        foreach (QFileInfo file, fileList)
        {
            icon = new QIcon(file.absoluteFilePath());
            ui->iconList->addItem(new QListWidgetItem(icon->pixmap(64, 64), file.fileName()));
            delete icon;
        }
    }
}
