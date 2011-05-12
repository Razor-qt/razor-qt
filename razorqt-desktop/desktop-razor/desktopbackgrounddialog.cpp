#include <QColorDialog>
#include <QFileDialog>
#include <QDirIterator>
#include <QDesktopWidget>
#include "desktopbackgrounddialog.h"
#include <razorqt/readsettings.h>


DesktopBackgroundDialog::DesktopBackgroundDialog(QSize desktopSize, QWidget * parent)
    : QDialog(parent),
      m_desktopSize(desktopSize),
      m_type(RazorWorkSpaceManager::BackgroundColor)
{
    setupUi(this);
    // center it to current desktop
    move(parent->geometry().center() - geometry().center());
    
    previewLabel->setScaledContents(false);
    previewLabel->setAlignment(Qt::AlignCenter);

    connect(colorButton, SIGNAL(clicked()),
            this, SLOT(colorButton_clicked()));
    connect(wallpaperButton, SIGNAL(clicked()),
            this, SLOT(wallpaperButton_clicked()));
    connect(systemButton, SIGNAL(clicked()),
            this, SLOT(systemButton_clicked()));
    connect(keepAspectCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(preview()));

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

DesktopBackgroundDialog::~DesktopBackgroundDialog()
{
}
    
QBrush DesktopBackgroundDialog::background()
{
    QBrush background;
    switch (m_type)
    {
        case (RazorWorkSpaceManager::BackgroundPixmap):
        {
            QPixmap pm(m_wallpaper);
            pm = pm.scaled(m_desktopSize,
                           keepAspectCheckBox->isChecked() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio,
                           Qt::SmoothTransformation);
            background.setTexture(pm);
            break;
        }
        default:
            background.setColor(m_color);
            background.setStyle(Qt::SolidPattern);
    }
    return background;
}

void DesktopBackgroundDialog::save(int screen, ReadSettings * cfg)
{
    QSettings * s = cfg->settings();
    s->beginGroup("razor");
    // It's strange. Event that I set array size to desktop count, there is always
    // index used. A bug in Qt? But it does not matter. I use screenCount()
    // in the array reading routine...
    s->beginWriteArray("desktops", QApplication::desktop()->screenCount());
    s->setArrayIndex(screen);
    if (m_type == RazorWorkSpaceManager::BackgroundColor)
    {
        s->setValue("wallpaper_type", "color");
        s->setValue("wallpaper", m_color.name());
    }
    else
    {
        s->setValue("wallpaper_type", "pixmap");
        s->setValue("wallpaper", m_wallpaper);
        s->setValue("keep_aspect_ratio", keepAspectCheckBox->isChecked());
    }
    s->endArray();
    s->endGroup();
}

void DesktopBackgroundDialog::colorButton_clicked()
{
    QColor c = QColorDialog::getColor(m_color, this, tr("Select Background Color"));
    if (!c.isValid())
        return;
    
    m_type = RazorWorkSpaceManager::BackgroundColor;
    m_color = c;
    preview();
}

void DesktopBackgroundDialog::wallpaperButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this,
                                                tr("Select Wallpaper Image"),
                                                QDir::currentPath(),
                                                tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (fname.isNull())
        return;
    
    m_type = RazorWorkSpaceManager::BackgroundPixmap;
    m_wallpaper = fname;
    preview();
}

void DesktopBackgroundDialog::systemButton_clicked()
{
    QString fname = QFileDialog::getOpenFileName(this,
                                                tr("Select Sytem Wallpaper Image"),
                                                "/usr/share/wallpapers",
                                                tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (fname.isNull())
        return;
    
    m_type = RazorWorkSpaceManager::BackgroundPixmap;
    m_wallpaper = fname;
    preview();
}

void DesktopBackgroundDialog::preview()
{
    switch (m_type)
    {
        case RazorWorkSpaceManager::BackgroundPixmap:
        {
            Qt::AspectRatioMode mode = ( keepAspectCheckBox->isChecked())? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio;
            QPixmap p = QPixmap(m_wallpaper).scaled(previewLabel->size(), mode, Qt::FastTransformation);
            previewLabel->setPixmap(p);
            break;
        }
        case RazorWorkSpaceManager::BackgroundColor:
        {
            QPixmap p(1, 1);
            p.fill(m_color);
            previewLabel->setPixmap(p);
            break;
        }
    }

    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
