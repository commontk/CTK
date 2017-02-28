/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

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

// STD includes
#include <cmath>

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
  QSize maxSizeHint(int* visibleItemsCount = 0)const;

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
QSize ctkFlowLayoutPrivate::maxSizeHint(int *visibleItemsCount)const
{
  if (visibleItemsCount)
    {
    *visibleItemsCount = 0;
    }
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
    if (visibleItemsCount)
      {
      ++*visibleItemsCount;
      }
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
  int length = 0;
  int max = this->Orientation == Qt::Horizontal ?
    effectiveRect.right() + 1 : effectiveRect.bottom() + 1;
  int maxX = left + right;
  int maxY = top + bottom;
  QSize maxItemSize = this->AlignItems ? this->maxSizeHint() : QSize();

  int spaceX = q->horizontalSpacing();
  int spaceY = q->verticalSpacing();
  int space = this->Orientation == Qt::Horizontal ? spaceX : spaceY;
  QLayoutItem* previousItem = NULL;
  foreach (QLayoutItem* item, this->ItemList)
    {
    QWidget *wid = item->widget();
    if (wid && wid->isHidden())
      {
      continue;
      }
    QPoint next = pos;
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
      // If justified alignment is requested then expand the last item in the row
      // to fill the available space. If the width of items were highly varying then
      // expanding width of all items proportionally could provide visually more
      // apealing results, but expanding only the last item was much simpler to implement,
      // and works very well most of the cases.
      if (!testOnly && q->alignment() == Qt::AlignJustify && previousItem)
        {
        QRect geometry = previousItem->geometry();
        geometry.adjust(0, 0, max + space - pos.x(), 0);
        previousItem->setGeometry(geometry);
        maxX = qMax(maxX, geometry.right() + right);
        }
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

    maxX = qMax( maxX , pos.x() + item->sizeHint().width() + right);
    maxY = qMax( maxY , pos.y() + item->sizeHint().height() + bottom);
    pos = next;
    length = qMax(length, this->Orientation == Qt::Horizontal ?
      itemSize.height() : itemSize.width());
    previousItem = item;
    }
  return this->Orientation == Qt::Horizontal ? maxY : maxX;
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
  return Qt::Vertical | Qt::Horizontal;
}

// --------------------------------------------------------------------------
bool ctkFlowLayout::hasWidthForHeight() const
{
  Q_D(const ctkFlowLayout);
  return d->Orientation == Qt::Vertical;
}

// --------------------------------------------------------------------------
int ctkFlowLayout::widthForHeight(int height) const
{
  Q_D(const ctkFlowLayout);
  QRect rect(0, 0, 0, height);
  int width = d->doLayout(rect, true);
  return width;
}

// --------------------------------------------------------------------------
bool ctkFlowLayout::hasHeightForWidth() const
{
  Q_D(const ctkFlowLayout);
  return d->Orientation == Qt::Horizontal;
}

// --------------------------------------------------------------------------
int ctkFlowLayout::heightForWidth(int width) const
{
  Q_D(const ctkFlowLayout);
  QRect rect(0, 0, width, 0);
  /// here we see the limitations of the vertical layout, it should be
  /// widthForHeight in this case.
  if (d->AlignItems && d->Orientation == Qt::Vertical)
    {
    int itemCount;
    QSize itemSize = d->maxSizeHint(&itemCount);
    QMargins margins = this->contentsMargins();
    int realWidth = width - margins.left() - margins.right();
    int itemCountPerRow = (realWidth + this->horizontalSpacing())
      / (itemSize.width() + this->horizontalSpacing());
    int rowCount = std::ceil( static_cast<float>(itemCount) / itemCountPerRow);
    rect.setHeight(rowCount * itemSize.height() +
                   (rowCount -1) * this->verticalSpacing() +
                   margins.top() + margins.bottom());
    }
  int height = d->doLayout(rect, true);
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

// --------------------------------------------------------------------------
ctkFlowLayout* ctkFlowLayout::replaceLayout(QWidget* widget)
{
  QLayout* oldLayout = widget->layout();

  ctkFlowLayout* flowLayout = new ctkFlowLayout;
  bool isVerticalLayout = qobject_cast<QVBoxLayout*>(oldLayout) != 0;
  flowLayout->setPreferredExpandingDirections(
    isVerticalLayout ? Qt::Vertical : Qt::Horizontal);
  flowLayout->setAlignItems(false);
  int margins[4];
  oldLayout->getContentsMargins(&margins[0],&margins[1],&margins[2],&margins[3]);
  QLayoutItem* item = 0;
  while((item = oldLayout->takeAt(0)))
    {
    if (item->widget())
      {
      flowLayout->addWidget(item->widget());
      }
    }
  // setLayout() will take care or reparenting layouts and widgets
  delete oldLayout;
  flowLayout->setContentsMargins(0,0,0,0);
  widget->setLayout(flowLayout);
  return flowLayout;
}
