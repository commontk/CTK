/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QStyle>
#include <QWidget>

// CTK includes
#include "ctkFlowLayout.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkFlowLayout");

//-----------------------------------------------------------------------------
class ctkFlowLayoutPrivate
{
  Q_DECLARE_PUBLIC(ctkFlowLayout);
protected:
  ctkFlowLayout* const q_ptr;
public:
  ctkFlowLayoutPrivate(ctkFlowLayout& object);
  void init();
  void deleteAll();

  int doLayout(const QRect &rect, bool testOnly) const;
  int smartSpacing(QStyle::PixelMetric pm) const;
  QSize maxSizeHint()const;

  QList<QLayoutItem *> ItemList;
  Qt::Orientation Orientation;
  int HorizontalSpacing;
  int VerticalSpacing;
  bool AlignItems;
  Qt::Orientations PreferredDirections;
};

// --------------------------------------------------------------------------
ctkFlowLayoutPrivate::ctkFlowLayoutPrivate(ctkFlowLayout& object)
  :q_ptr(&object)
{
  this->HorizontalSpacing = -1;
  this->VerticalSpacing = -1;
  this->Orientation = Qt::Horizontal;
  this->PreferredDirections = Qt::Horizontal | Qt::Vertical;
  this->AlignItems = true;
}

// --------------------------------------------------------------------------
void ctkFlowLayoutPrivate::init()
{
  Q_Q(ctkFlowLayout);
}

// --------------------------------------------------------------------------
void ctkFlowLayoutPrivate::deleteAll()
{
  Q_Q(ctkFlowLayout);
  foreach(QLayoutItem* item, this->ItemList)
    {
    delete item;
    }
  this->ItemList.clear();
  q->invalidate();
}

// --------------------------------------------------------------------------
QSize ctkFlowLayoutPrivate::maxSizeHint()const
{
  QSize maxItemSize;
  foreach (QLayoutItem* item, this->ItemList)
    {
    QWidget *wid = item->widget();
    if (wid && !wid->isVisibleTo(wid->parentWidget()))
      {// don't take into account hidden items
      continue;
      }
    maxItemSize.rwidth() = qMax(item->sizeHint().width(), maxItemSize.width());
    maxItemSize.rheight() = qMax(item->sizeHint().height(), maxItemSize.height());
    }
  return maxItemSize;
}

// --------------------------------------------------------------------------
int ctkFlowLayoutPrivate::doLayout(const QRect& rect, bool testOnly)const
{
  Q_Q(const ctkFlowLayout);
  int left, top, right, bottom;
  q->getContentsMargins(&left, &top, &right, &bottom);
  QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
  QPoint pos = QPoint(effectiveRect.x(), effectiveRect.y());
  QPoint next = pos;
  int length = 0;
  int max = this->Orientation == Qt::Horizontal ?
    effectiveRect.right() + 1 : effectiveRect.bottom() + 1;

  QSize maxItemSize = this->AlignItems ? this->maxSizeHint() : QSize();

  int spaceX = q->horizontalSpacing();
  int spaceY = q->verticalSpacing();
  int space = this->Orientation == Qt::Horizontal ? spaceX : spaceY;
  foreach (QLayoutItem* item, this->ItemList)
    {
    QWidget *wid = item->widget();
    if (wid && wid->isHidden())
      {
      continue;
      }
    next = pos;
    QSize itemSize = this->AlignItems ? maxItemSize : item->sizeHint();
    if (this->Orientation == Qt::Horizontal)
      {
      next += QPoint(itemSize.width() + spaceX, 0);
      }
    else
      {
      next += QPoint(0, itemSize.height() + spaceY);
      }
    if (this->Orientation == Qt::Horizontal &&
        (next.x() - space > max) && length > 0)
      {
      pos = QPoint(effectiveRect.x(), pos.y() + length + space);
      next = pos + QPoint(itemSize.width() + space, 0);
      length = 0;
      }
    else if (this->Orientation == Qt::Vertical &&
          (next.y() - space > max) && length > 0)
      {
      pos = QPoint( pos.x() + length + space, effectiveRect.y());
      next = pos + QPoint(0, itemSize.height() + space);
      length = 0;
      }

    if (!testOnly)
      {
      item->setGeometry(QRect(pos, item->sizeHint()));
      }

    pos = next;
    length = qMax(length, this->Orientation == Qt::Horizontal ?
      itemSize.height() : itemSize.width());
    }
  return pos.y() + length - rect.y() + bottom;
}

//-----------------------------------------------------------------------------
int ctkFlowLayoutPrivate::smartSpacing(QStyle::PixelMetric pm) const
{
  Q_Q(const ctkFlowLayout);
  QObject* parentObject = q->parent();
  if (!parentObject)
    {
    return -1;
    }
  else if (parentObject->isWidgetType())
    {
    QWidget* parentWidget = qobject_cast<QWidget *>(parentObject);
    return parentWidget->style()->pixelMetric(pm, 0, parentWidget);
    }
  else
    {
    return static_cast<QLayout *>(parentObject)->spacing();
    }
}

// --------------------------------------------------------------------------
ctkFlowLayout::ctkFlowLayout(Qt::Orientation orientation, QWidget *parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkFlowLayoutPrivate(*this))
{
  Q_D(ctkFlowLayout);
  d->init();
  this->setOrientation(orientation);
}

// --------------------------------------------------------------------------
ctkFlowLayout::ctkFlowLayout(QWidget *parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkFlowLayoutPrivate(*this))
{
  Q_D(ctkFlowLayout);
  d->init();
}

// --------------------------------------------------------------------------
ctkFlowLayout::ctkFlowLayout()
  : d_ptr(new ctkFlowLayoutPrivate(*this))
{
  Q_D(ctkFlowLayout);
  d->init();
}

// --------------------------------------------------------------------------
ctkFlowLayout::~ctkFlowLayout()
{
  Q_D(ctkFlowLayout);
  d->deleteAll();
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setOrientation(Qt::Orientation orientation)
{
  Q_D(ctkFlowLayout);
  d->Orientation = orientation;
  this->invalidate();
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setPreferredExpandingDirections(Qt::Orientations directions)
{
  Q_D(ctkFlowLayout);
  d->PreferredDirections = directions;
}

// --------------------------------------------------------------------------
Qt::Orientations ctkFlowLayout::preferredExpandingDirections()const
{
  Q_D(const ctkFlowLayout);
  return d->PreferredDirections;
}
  
// --------------------------------------------------------------------------
Qt::Orientation ctkFlowLayout::orientation() const
{
  Q_D(const ctkFlowLayout);
  return d->Orientation;
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setHorizontalSpacing(int spacing)
{
  Q_D(ctkFlowLayout);
  d->HorizontalSpacing = spacing;
  this->invalidate();
}

// --------------------------------------------------------------------------
int ctkFlowLayout::horizontalSpacing() const
{
  Q_D(const ctkFlowLayout);
  if (d->HorizontalSpacing < 0)
    {
    return d->smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
  return d->HorizontalSpacing;
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setVerticalSpacing(int spacing)
{
  Q_D(ctkFlowLayout);
  d->VerticalSpacing = spacing;
  this->invalidate();
}

// --------------------------------------------------------------------------
int ctkFlowLayout::verticalSpacing() const
{
  Q_D(const ctkFlowLayout);
  if (d->VerticalSpacing < 0)
    {
    return d->smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
  return d->VerticalSpacing;
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setAlignItems(bool align)
{
  Q_D(ctkFlowLayout);
  d->AlignItems = align;
  this->invalidate();
}

// --------------------------------------------------------------------------
bool ctkFlowLayout::alignItems() const
{
  Q_D(const ctkFlowLayout);
  return d->AlignItems;
}

// --------------------------------------------------------------------------
void ctkFlowLayout::addItem(QLayoutItem *item)
{
  Q_D(ctkFlowLayout);
  d->ItemList << item;
  this->invalidate();
}

// --------------------------------------------------------------------------
Qt::Orientations ctkFlowLayout::expandingDirections() const
{
  Q_D(const ctkFlowLayout);
  //return d->Orientation;
  return Qt::Vertical | Qt::Horizontal;
}

// --------------------------------------------------------------------------
bool ctkFlowLayout::hasHeightForWidth() const
{
  return true;
}

// --------------------------------------------------------------------------
int ctkFlowLayout::heightForWidth(int width) const
{
  Q_D(const ctkFlowLayout);
  /// here we see the limitations of the vertical layout, it should be
  /// widthForHeight in this case.
  int height = d->doLayout(QRect(0, 0, width, 0), true);
  return height;
}

// --------------------------------------------------------------------------
int ctkFlowLayout::count() const
{
  Q_D(const ctkFlowLayout);
  return d->ItemList.count();
}

// --------------------------------------------------------------------------
QLayoutItem *ctkFlowLayout::itemAt(int index) const
{
  Q_D(const ctkFlowLayout);
  if (index < 0 || index >= this->count())
    {
    return 0;
    }
  return d->ItemList[index];
}

// --------------------------------------------------------------------------
QSize ctkFlowLayout::minimumSize() const
{
  Q_D(const ctkFlowLayout);
  QSize size;
  foreach(QLayoutItem* item, d->ItemList)
    {
    QWidget* widget = item->widget();
    if (widget && !widget->isVisibleTo(widget->parentWidget()))
      {
      continue;
      }
    size = size.expandedTo(item->minimumSize());
    }
  int left, top, right, bottom;
  this->getContentsMargins(&left, &top, &right, &bottom);
  size += QSize(left+right, top+bottom);
  return size;
}

// --------------------------------------------------------------------------
void ctkFlowLayout::setGeometry(const QRect &rect)
{
  Q_D(ctkFlowLayout);
  this->QLayout::setGeometry(rect);
  d->doLayout(rect, false);
}

// --------------------------------------------------------------------------
QSize ctkFlowLayout::sizeHint() const
{
  Q_D(const ctkFlowLayout);
  QSize size = QSize(0,0);
  int countX = 0;
  int countY = 0;
  QSize maxSizeHint = d->AlignItems ? d->maxSizeHint() : QSize();
  // Add items
  foreach (QLayoutItem* item, d->ItemList)
    {
    QWidget* widget = item->widget();
    if (widget && !widget->isVisibleTo(widget->parentWidget()))
      {
      continue;
      }
    QSize itemSize = d->AlignItems ? maxSizeHint : item->sizeHint();
    Qt::Orientation grow;
    if (d->PreferredDirections & Qt::Horizontal &&
        !(d->PreferredDirections & Qt::Vertical))
      {
      grow = Qt::Horizontal;
      }
    else if (d->PreferredDirections & Qt::Vertical &&
             !(d->PreferredDirections & Qt::Horizontal))
      {
      grow = Qt::Vertical;
      }
    else
      {
      grow = countY >= countX ? Qt::Horizontal : Qt::Vertical;
      }
    if (grow == Qt::Horizontal)
      {
      size.rwidth() += itemSize.width();
      size.rheight() = qMax(itemSize.height(), size.height());
      ++countX;
      }
    else
      {
      size.rwidth() = qMax(itemSize.width(), size.width());
      size.rheight() += itemSize.height();
      ++countY;
      }
    }
  // Add spacing
  size += QSize((countX-1) * this->horizontalSpacing(),
                (countY-1) * this->verticalSpacing());
  // Add margins
  int left, top, right, bottom;
  this->getContentsMargins(&left, &top, &right, &bottom);
  size += QSize(left+right, top+bottom);
  return size;
}

// --------------------------------------------------------------------------
QLayoutItem *ctkFlowLayout::takeAt(int index)
{
  Q_D(ctkFlowLayout);
  if (index < 0 || index >= this->count())
    {
    return 0;
    }
  QLayoutItem* item = d->ItemList.takeAt(index);
  this->invalidate();
  return item;
}
