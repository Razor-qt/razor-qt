/*
 * Copyright © 2003-2007 Fredrik Höglund <fredrik@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 as published by the Free Software Foundation.
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

#include <QPainter>
#include <QMouseEvent>

#include "previewwidget.h"

#include "crtheme.h"

#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>


namespace {
  // Preview cursors
  const char *const cursorNames[] = {
    "left_ptr",
    "left_ptr_watch",
    "wait",
    "pointing_hand",
    "whats_this",
    "ibeam",
    "size_all",
    "size_fdiag",
    "cross",
    "split_h",
    "size_ver",
    "size_hor",
    "size_bdiag",
    "split_v",
  };

  const int numCursors = 9;       // The number of cursors from the above list to be previewed
  const int previewSize = 24;     // The nominal cursor size to be used in the preview widget
  const int cursorSpacing = 20;   // Spacing between preview cursors
  const int widgetMinWidth = 10;  // The minimum width of the preview widget
  const int widgetMinHeight = 48; // The minimum height of the preview widget
}


///////////////////////////////////////////////////////////////////////////////
class PreviewCursor {
public:
  PreviewCursor (const XCursorThemeData &theme, const QString &name);
  ~PreviewCursor () {}

  const QPixmap &pixmap () const { return mPixmap; }
  Cursor handle () const { return mCursor.handle(); }
  int width () const { return mPixmap.width(); }
  int height () const { return mPixmap.height(); }
  inline QRect rect () const;
  void setPosition (const QPoint &p) { mPos = p; }
  void setPosition (int x, int y) { mPos = QPoint(x, y); }
  QPoint position () const { return mPos; }
  operator const QCursor& () const { return mCursor; }
  operator const QPixmap& () const { return pixmap(); }

private:
  QPixmap mPixmap;
  QCursor mCursor;
  QPoint mPos;
};


///////////////////////////////////////////////////////////////////////////////
PreviewCursor::PreviewCursor (const XCursorThemeData &theme, const QString &name) {
  // Create the preview pixmap
  QImage image = theme.loadImage(name, previewSize);
  if (image.isNull()) return;
  int maxSize = previewSize*2;
  if (image.height() > maxSize || image.width() > maxSize)
    image = image.scaled(maxSize, maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  mPixmap = QPixmap::fromImage(image);
  // load the cursor
  mCursor = theme.loadCursor(name, previewSize);
}


QRect PreviewCursor::rect () const {
  return QRect(mPos, mPixmap.size()).adjusted(-(cursorSpacing/2), -(cursorSpacing/2), cursorSpacing/2, cursorSpacing/2);
}


///////////////////////////////////////////////////////////////////////////////
PreviewWidget::PreviewWidget (QWidget *parent) : QWidget(parent) {
  setMouseTracking(true);
  mCurrent = NULL;
}


PreviewWidget::~PreviewWidget () {
  qDeleteAll(mList);
  mList.clear();
}


QSize PreviewWidget::sizeHint () const {
  int totalWidth = 0, maxHeight = 0;
  foreach (const PreviewCursor *c, mList) {
    totalWidth += c->width();
    maxHeight = qMax(c->height(), maxHeight);
  }
  totalWidth += (mList.count()-1)*cursorSpacing;
  maxHeight = qMax(maxHeight, widgetMinHeight);
  return QSize(qMax(totalWidth, widgetMinWidth), qMax(height(), maxHeight));
}


void PreviewWidget::layoutItems () {
  if (!mList.isEmpty()) {
    QSize size = sizeHint();
    int cursorWidth = size.width()/mList.count();
    int nextX = (width()-size.width())/2;
    foreach (PreviewCursor *c, mList) {
      c->setPosition(nextX+(cursorWidth-c->width())/2, (height()-c->height())/2);
      nextX += cursorWidth;
    }
  }
  mNeedLayout = false;
}


void PreviewWidget::setTheme (const XCursorThemeData &theme) {
  qDeleteAll(mList);
  mList.clear();
  for (int i = 0; i < numCursors; i++) mList << new PreviewCursor(theme, cursorNames[i]);
  mNeedLayout = true;
  updateGeometry();
  mCurrent = NULL;
  update();
}


void PreviewWidget::clearTheme () {
  qDeleteAll(mList);
  mList.clear();
  mCurrent = NULL;
  update();
}


void PreviewWidget::paintEvent (QPaintEvent *) {
  QPainter p(this);
  if (mNeedLayout) layoutItems();
  foreach (const PreviewCursor *c, mList) {
    if (c->pixmap().isNull()) continue;
    p.drawPixmap(c->position(), *c);
  }
}


void PreviewWidget::mouseMoveEvent (QMouseEvent *e) {
  if (mNeedLayout) layoutItems();
  foreach (const PreviewCursor *c, mList) {
    if (c->rect().contains(e->pos())) {
      if (c != mCurrent) {
        setCursor(*c);
        mCurrent = c;
      }
      return;
    }
  }
  setCursor(Qt::ArrowCursor);
  mCurrent = NULL;
}


void PreviewWidget::resizeEvent (QResizeEvent *) {
  if (!mList.isEmpty()) mNeedLayout = true;
}
