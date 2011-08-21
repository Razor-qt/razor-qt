/*
 * Copyright © 2005-2007 Fredrik Höglund <fredrik@kde.org>
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

#include "thememodel.h"

#include <QDir>

#include "crtheme.h"
#include "cfgfile.h"

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>


//#define DUMP_FOUND_THEMES


///////////////////////////////////////////////////////////////////////////////
XCursorThemeModel::XCursorThemeModel (QObject *parent) : QAbstractTableModel(parent) {
  insertThemes();
}


XCursorThemeModel::~XCursorThemeModel () {
  qDeleteAll(mList);
  mList.clear();
}


QVariant XCursorThemeModel::headerData (int section, Qt::Orientation orientation, int role) const {
  // Only provide text for the headers
  if (role != Qt::DisplayRole) return QVariant();
  // Horizontal header labels
  if (orientation == Qt::Horizontal) {
    switch (section) {
      case NameColumn: return tr("Name");
      case DescColumn: return tr("Description");
      default: return QVariant();
    }
  }
  // Numbered vertical header lables
  return QString(section);
}


QVariant XCursorThemeModel::data (const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= mList.count()) return QVariant();
  const XCursorThemeData *theme = mList.at(index.row());
  // Text label
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case NameColumn: return theme->title();
      case DescColumn: return theme->description();
      default: return QVariant();
    }
  }
  // Description for the first name column
  if (role == XCursorThemeData::DisplayDetailRole && index.column() == NameColumn) return theme->description();
  // Icon for the name column
  if (role == Qt::DecorationRole && index.column() == NameColumn) return theme->icon();
  //
  return QVariant();
}


void XCursorThemeModel::sort (int column, Qt::SortOrder order) {
  Q_UNUSED(column);
  Q_UNUSED(order);
  // Sorting of the model isn't implemented, as the KCM currently uses
  // a sorting proxy model.
}


const XCursorThemeData *XCursorThemeModel::theme (const QModelIndex &index) {
  if (!index.isValid()) return NULL;
  if (index.row() < 0 || index.row() >= mList.count()) return NULL;
  return mList.at(index.row());
}


QModelIndex XCursorThemeModel::findIndex (const QString &name) {
  uint hash = qHash(name);
  for (int i = 0; i < mList.count(); i++) {
    const XCursorThemeData *theme = mList.at(i);
    if (theme->hash() == hash) return index(i, 0);
  }
  return QModelIndex();
}


QModelIndex XCursorThemeModel::defaultIndex () {
  return findIndex(mDefaultName);
}


const QStringList XCursorThemeModel::searchPaths () {
  if (!mBaseDirs.isEmpty()) return mBaseDirs;
  // Get the search path from Xcursor
  QString path = XcursorLibraryPath();
  // Separate the paths
  mBaseDirs = path.split(':', QString::SkipEmptyParts);
  // Remove duplicates
  QMutableStringListIterator i(mBaseDirs);
  while (i.hasNext()) {
    const QString path = i.next();
    QMutableStringListIterator j(i);
    while (j.hasNext()) if (j.next() == path) j.remove();
  }
  // Expand all occurrences of ~/ to the home dir
  mBaseDirs.replaceInStrings(QRegExp("^~\\/"), QDir::home().path() + '/');
  return mBaseDirs;
}


bool XCursorThemeModel::hasTheme (const QString &name) const {
  const uint hash = qHash(name);
  foreach (const XCursorThemeData *theme, mList) if (theme->hash() == hash) return true;
  return false;
}


bool XCursorThemeModel::isCursorTheme (const QString &theme, const int depth) {
  // Prevent infinite recursion
  if (depth > 10) return false;
  // Search each icon theme directory for 'theme'
  foreach (const QString &baseDir, searchPaths()) {
    QDir dir(baseDir);
    if (!dir.exists() || !dir.cd(theme)) continue;
    // If there's a cursors subdir, we'll assume this is a cursor theme
    if (dir.exists("cursors")) return true;
    // If the theme doesn't have an index.theme file, it can't inherit any themes
    if (!dir.exists("index.theme")) continue;
    // Open the index.theme file, so we can get the list of inherited themes
    QMultiMap<QString, QString> cfg = loadCfgFile(dir.path()+"/index.theme", true);
    QStringList inherits = cfg.values("icon theme/inherits");
    // Recurse through the list of inherited themes, to check if one of them is a cursor theme
    // note that items are reversed
    for (int f = inherits.size()-1; f >= 0; f--) {
      QString inh = inherits.at(f);
      // Avoid possible DoS
      if (inh == theme) continue;
      if (isCursorTheme(inh, depth+1)) return true;
    }
  }
  return false;
}


bool XCursorThemeModel::handleDefault (const QDir &themeDir) {
  QFileInfo info(themeDir.path());
  // If "default" is a symlink
  if (info.isSymLink()) {
    QFileInfo target(info.symLinkTarget());
    if (target.exists() && (target.isDir() || target.isSymLink())) mDefaultName = target.fileName();
    return true;
  }
  // If there's no cursors subdir, or if it's empty
  if (!themeDir.exists("cursors") || QDir(themeDir.path() + "/cursors").entryList(QDir::Files | QDir::NoDotAndDotDot).isEmpty()) {
    if (themeDir.exists("index.theme")) {
      XCursorThemeData theme(themeDir);
      if (!theme.inherits().isEmpty()) mDefaultName = theme.inherits().at(0);
    }
    return true;
  }
  mDefaultName = QLatin1String("default");
  return false;
}


void XCursorThemeModel::processThemeDir (const QDir &themeDir) {
#ifdef DUMP_FOUND_THEMES
  qDebug() << "looking at:" << themeDir.path();
#endif
  bool haveCursors = themeDir.exists("cursors");
  // Special case handling of "default", since it's usually either a
  // symlink to another theme, or an empty theme that inherits another theme
  if (mDefaultName.isNull() && themeDir.dirName() == "default") {
    if (handleDefault(themeDir)) return;
  }
  // If the directory doesn't have a cursors subdir and lacks an
  // index.theme file it can't be a cursor theme.
  if (!themeDir.exists("index.theme") && !haveCursors) return;
  // Create a cursor theme object for the theme dir
  XCursorThemeData *theme = new XCursorThemeData(themeDir);
  // Skip this theme if it's hidden
#ifdef DUMP_FOUND_THEMES
  qDebug() <<
    "  theme name:" << theme->name() <<
    "\n  theme title:" << theme->title() <<
    "\n  theme desc:" << theme->description() <<
    "\n  theme sample:" << theme->sample() <<
    "\n  theme inherits:" << theme->inherits()
  ;
#endif
  if (theme->isHidden()) {
    delete theme;
    return;
  }
  // If there's no cursors subdirectory we'll do a recursive scan
  // to check if the theme inherits a theme with one
  if (!haveCursors) {
    bool foundCursorTheme = false;
    foreach (const QString &name, theme->inherits()) if ((foundCursorTheme = isCursorTheme(name))) break;
    if (!foundCursorTheme) {
      delete theme;
      return;
    }
  }
  // Append the theme to the list
  mList.append(theme);
}


void XCursorThemeModel::insertThemes () {
  // Scan each base dir for Xcursor themes and add them to the list
  foreach (const QString &baseDir, searchPaths()) {
    QDir dir(baseDir);
    if (!dir.exists()) continue;
    // Process each subdir in the directory
    foreach (const QString &name, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::Readable | QDir::Executable)) {
      // Don't process the theme if a theme with the same name already exists
      // in the list. Xcursor will pick the first one it finds in that case,
      // and since we use the same search order, the one Xcursor picks should
      // be the one already in the list
      if (hasTheme(name)) {
        //k8:qDebug() << "duplicate theme:" << dir.path()+name;
      }
      if (!dir.cd(name)) {
        //k8:qDebug() << "can't cd:" << dir.path()+name;
        continue;
      }
      processThemeDir(dir);
      dir.cdUp(); // Return to the base dir
    }
  }
  // The theme Xcursor will end up using if no theme is configured
  //if (mDefaultName.isNull() || !hasTheme(mDefaultName)) mDefaultName = legacy->name();
}


bool XCursorThemeModel::addTheme (const QDir &dir) {
  XCursorThemeData *theme = new XCursorThemeData(dir);
  // Don't add the theme to the list if it's hidden
  if (theme->isHidden()) {
    delete theme;
    return false;
  }
  // If an item with the same name already exists in the list,
  // we'll remove it before inserting the new one.
  for (int i = 0; i < mList.count(); i++) {
    if (mList.at(i)->hash() == theme->hash()) {
      removeTheme(index(i, 0));
      break;
    }
  }
  // Append the theme to the list
  beginInsertRows(QModelIndex(), rowCount(), rowCount());
  mList.append(theme);
  endInsertRows();
  return true;
}


void XCursorThemeModel::removeTheme (const QModelIndex &index) {
  if (!index.isValid()) return;
  beginRemoveRows(QModelIndex(), index.row(), index.row());
  delete mList.takeAt(index.row());
  endRemoveRows();
}
