/* Copyright © 2006-2007 Fredrik Höglund <fredrik@kde.org>
 * (c)GPL2 (c)GPL3
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 or at your option version 3 as published 
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
/*
 * additional code: Ketmar // Vampire Avalon (psyc://ketmar.no-ip.org/~Ketmar)
 */
#include <QDebug>

#include "crtheme.h"

#include <QStyle>
#include <QX11Info>

#include "cfgfile.h"

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/Xfixes.h>

// Static variable holding alternative names for some cursors
static QHash<QString, QString> alternatives;


///////////////////////////////////////////////////////////////////////////////
XCursorThemeData::XCursorThemeData (const QDir &aDir) {
    mHidden = false;
  // parse configs, etc
  mPath = aDir.path();
  setName(aDir.dirName());
  if (aDir.exists("index.theme")) parseIndexFile();
  if (mDescription.isEmpty()) mDescription = "no description";
  if (mTitle.isEmpty()) mTitle = mName;
}


void XCursorThemeData::parseIndexFile () {
  QMultiMap<QString, QString> cfg = loadCfgFile(mPath+"/index.theme", true);
  if (cfg.contains("icon theme/name")) mTitle = cfg.values("icon theme/name").at(0).trimmed();
  if (cfg.contains("icon theme/comment")) mDescription = cfg.values("icon theme/comment").at(0).trimmed();
  if (cfg.contains("icon theme/example")) mSample = cfg.values("icon theme/example").at(0).trimmed();
  if (cfg.contains("icon theme/hidden")) {
      QString hiddenValue = cfg.values("icon theme/hidden").at(0).toLower();
      mHidden = hiddenValue=="false" ? false : true;
  }
  if (cfg.contains("icon theme/inherits")) {
    QStringList i = cfg.values("icon theme/inherits"), res;
    for (int f = i.size()-1; f >= 0; f--) res << i.at(f).trimmed();
  }
  if (mDescription.startsWith("- Converted by")) mDescription.remove(0, 2);
}


QString XCursorThemeData::findAlternative (const QString &name) const {
  if (alternatives.isEmpty()) {
    alternatives.reserve(18);

    // Qt uses non-standard names for some core cursors.
    // If Xcursor fails to load the cursor, Qt creates it with the correct name using the
    // core protcol instead (which in turn calls Xcursor). We emulate that process here.
    // Note that there's a core cursor called cross, but it's not the one Qt expects.
    alternatives.insert("cross",         "crosshair");
    alternatives.insert("up_arrow",      "center_ptr");
    alternatives.insert("wait",          "watch");
    alternatives.insert("ibeam",         "xterm");
    alternatives.insert("size_all",      "fleur");
    alternatives.insert("pointing_hand", "hand2");

    // Precomputed MD5 hashes for the hardcoded bitmap cursors in Qt and KDE.
    // Note that the MD5 hash for left_ptr_watch is for the KDE version of that cursor.
    alternatives.insert("size_ver",       "00008160000006810000408080010102");
    alternatives.insert("size_hor",       "028006030e0e7ebffc7f7070c0600140");
    alternatives.insert("size_bdiag",     "c7088f0f3e6c8088236ef8e1e3e70000");
    alternatives.insert("size_fdiag",     "fcf1c3c7cd4491d801f1e1c78f100000");
    alternatives.insert("whats_this",     "d9ce0ab605698f320427677b458ad60b");
    alternatives.insert("split_h",        "14fef782d02440884392942c11205230");
    alternatives.insert("split_v",        "2870a09082c103050810ffdffffe0204");
    alternatives.insert("forbidden",      "03b6e0fcb3499374a867c041f52298f0");
    alternatives.insert("left_ptr_watch", "3ecb610c1bf2410f44200f48c40d3599");
    alternatives.insert("hand2",          "e29285e634086352946a0e7090d73106");
    alternatives.insert("openhand",       "9141b49c8149039304290b508d208c40");
    alternatives.insert("closedhand",     "05e88622050804100c20044008402080");
  }

  return alternatives.value(name, QString());
}


QPixmap XCursorThemeData::icon () const {
  if (mIcon.isNull()) mIcon = createIcon();
  return mIcon;
}


QImage XCursorThemeData::autoCropImage (const QImage &image) const {
  // Compute an autocrop rectangle for the image
  QRect r(image.rect().bottomRight(), image.rect().topLeft());
  const quint32 *pixels = reinterpret_cast<const quint32*>(image.bits());
  for (int y = 0; y < image.height(); y++) {
    for (int x = 0; x < image.width(); x++) {
      if (*(pixels++)) {
        if (x < r.left()) r.setLeft(x);
        if (x > r.right()) r.setRight(x);
        if (y < r.top()) r.setTop(y);
        if (y > r.bottom()) r.setBottom(y);
      }
    }
  }
  // Normalize the rectangle
  return image.copy(r.normalized());
}


static int nominalCursorSize (int iconSize) {
  for (int i = 512; i > 8; i /= 2) {
    if (i < iconSize) return i;
    if ((i*0.75) < iconSize) return int(i*0.75);
  }
  return 8;
}


QPixmap XCursorThemeData::createIcon () const {
  int iconSize = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
  int cursorSize = nominalCursorSize(iconSize);
  QSize size = QSize(iconSize, iconSize);

  QPixmap pixmap;
  QImage image = loadImage(sample(), cursorSize);
  if (image.isNull() && sample() != "left_ptr") image = loadImage("left_ptr", cursorSize);
  if (!image.isNull()) {
    // Scale the image if it's larger than the preferred icon size
    if (image.width() > size.width() || image.height() > size.height())
      image = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    pixmap = QPixmap::fromImage(image);
  }
  return pixmap;
}


XcursorImage *XCursorThemeData::xcLoadImage (const QString &image, int size) const {
  QByteArray cursorName = QFile::encodeName(image);
  QByteArray themeName  = QFile::encodeName(name());
  return XcursorLibraryLoadImage(cursorName, themeName, size);
}


XcursorImages *XCursorThemeData::xcLoadImages (const QString &image, int size) const {
  QByteArray cursorName = QFile::encodeName(image);
  QByteArray themeName  = QFile::encodeName(name());
  return XcursorLibraryLoadImages(cursorName, themeName, size);
}


QCursor XCursorThemeData::loadCursor (const QString &name, int size) const {
  if (size == -1) size = XcursorGetDefaultSize(QX11Info::display());
  // Load the cursor images
  XcursorImages *images = xcLoadImages(name, size);
  if (!images) images = xcLoadImages(findAlternative(name), size);
  // Fall back to a legacy cursor
  //if (!images) return LegacyTheme::loadCursor(name);
  if (!images) return false;
  // Create the cursor
  Cursor handle = XcursorImagesLoadCursor(QX11Info::display(), images);
  QCursor cursor = QCursor(Qt::HANDLE(handle)); // QCursor takes ownership of the handle
  XcursorImagesDestroy(images);
  //setCursorName(cursor, name);
  return cursor;
}


QImage XCursorThemeData::loadImage (const QString &name, int size) const {
  if (size == -1) size = XcursorGetDefaultSize(QX11Info::display());
  // Load the image
  XcursorImage *xcimage = xcLoadImage(name, size);
  if (!xcimage) xcimage = xcLoadImage(findAlternative(name), size);
  // Fall back to a legacy cursor
  //if (!xcimage) return LegacyTheme::loadImage(name);
  if (!xcimage) return QImage();
  // Convert the XcursorImage to a QImage, and auto-crop it
  QImage image((uchar *)xcimage->pixels, xcimage->width, xcimage->height, QImage::Format_ARGB32_Premultiplied);
  image = autoCropImage(image);
  XcursorImageDestroy(xcimage);
  return image;
}


bool XCursorThemeData::isWritable () const {
  QFileInfo fi(path());
  return fi.isWritable();
}


///////////////////////////////////////////////////////////////////////////////
bool haveXfixes () {
  bool result = false;
  int event_base, error_base;
  if (XFixesQueryExtension(QX11Info::display(), &event_base, &error_base)) {
    int major, minor;
    XFixesQueryVersion(QX11Info::display(), &major, &minor);
    result = (major >= 2);
  }
  return result;
}


bool applyTheme (const XCursorThemeData &theme) {
  // Require the Xcursor version that shipped with X11R6.9 or greater, since
  // in previous versions the Xfixes code wasn't enabled due to a bug in the
  // build system (freedesktop bug #975).
  if (!haveXfixes()) return false;

  QByteArray themeName = QFile::encodeName(theme.name());

  // Set up the proper launch environment for newly started apps
  //k8:!!!:KToolInvocation::klauncher()->setLaunchEnv("XCURSOR_THEME", themeName);

  // Update the Xcursor X resources
  //k8:!!!:runRdb(0);

  // Reload the standard cursors
  QStringList names;
  // Qt cursors
  names << "left_ptr"       << "up_arrow"      << "cross"      << "wait"
        << "left_ptr_watch" << "ibeam"         << "size_ver"   << "size_hor"
        << "size_bdiag"     << "size_fdiag"    << "size_all"   << "split_v"
        << "split_h"        << "pointing_hand" << "openhand"
        << "closedhand"     << "forbidden"     << "whats_this";
  // X core cursors
  names << "X_cursor"            << "right_ptr"           << "hand1"
        << "hand2"               << "watch"               << "xterm"
        << "crosshair"           << "left_ptr_watch"      << "center_ptr"
        << "sb_h_double_arrow"   << "sb_v_double_arrow"   << "fleur"
        << "top_left_corner"     << "top_side"            << "top_right_corner"
        << "right_side"          << "bottom_right_corner" << "bottom_side"
        << "bottom_left_corner"  << "left_side"           << "question_arrow"
        << "pirate";

  //QX11Info x11Info;
  foreach (const QString &name, names) {
    QCursor cursor = theme.loadCursor(name);
    XFixesChangeCursorByName(QX11Info::display(), cursor.handle(), QFile::encodeName(name));
  }

  return true;
}


QString getCurrentTheme () {
  return XcursorGetTheme(QX11Info::display());
}
