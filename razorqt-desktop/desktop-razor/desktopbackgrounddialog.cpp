#include <QColorDialog>
#include <QFileDialog>
#include <QDirIterator>
#include "desktopbackgrounddialog.h"
#include <razorqt/readsettings.h>


DesktopBackgroundDialog::DesktopBackgroundDialog(QSize desktopSize, QWidget * parent)
    : QDialog(parent),
      m_desktopSize(desktopSize),
      m_type(RazorWorkSpaceManager::BackgroundColor)
{
    setupUi(this);
    
    connect(colorButton, SIGNAL(clicked()),
            this, SLOT(colorButton_clicked()));
    connect(wallpaperButton, SIGNAL(clicked()),
            this, SLOT(wallpaperButton_clicked()));
    connect(systemButton, SIGNAL(clicked()),
            this, SLOT(systemButton_clicked()));
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
    s->beginWriteArray("desktops");
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
    QPixmap p(1, 1);
    p.fill(c);
    previewLabel->setPixmap(p);
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
    QPixmap p(fname);
    previewLabel->setPixmap(p);
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
    QPixmap p(fname);
    previewLabel->setPixmap(p);

#if 0
    // this is probably too much complicated for such small feature... and mainly - it's deadly slow without any caching
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    // read system-wide wallpapers
    
    QListWidget systemListWidget;

    QDirIterator it("/usr/share/wallpapers", QDir::Files|QDir::Readable|QDir::NoDotDot, QDirIterator::Subdirectories);
    QFileInfo fi;
    while (it.hasNext())
    {
        QString fname(it.next());
        if (fname.endsWith(".png", Qt::CaseInsensitive)
            || fname.endsWith(".jpg", Qt::CaseInsensitive)
            || fname.endsWith(".jpeg", Qt::CaseInsensitive)
           )
        {
            fi.setFile(fname);
            QPixmap pm = QPixmap(fname).scaledToHeight(160);
            QListWidgetItem * item = new QListWidgetItem(pm, fi.fileName(), &systemListWidget);
            item->setData(Qt::UserRole, fname);
            systemListWidget.addItem(item);
        }
    }
    systemListWidget.show();

    QApplication::restoreOverrideCursor();
#endif
}

