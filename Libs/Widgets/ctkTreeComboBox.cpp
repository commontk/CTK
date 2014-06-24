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
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLayout>
#include <QScrollBar>
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
# include <QInputContext>
#endif
#include <QMouseEvent>
#include <QModelIndex>
#include <QStack>
#include <QTreeView>
#include <QDebug>

// CTK includes
#include "ctkTreeComboBox.h"

// -------------------------------------------------------------------------
class ctkTreeComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkTreeComboBox);
protected:
  ctkTreeComboBox* const q_ptr;
public:
  ctkTreeComboBoxPrivate(ctkTreeComboBox& object);
  int computeWidthHint()const;

  bool SkipNextHide;
  bool RootSet;
  bool SendCurrentItem;
  QPersistentModelIndex Root;
  int VisibleModelColumn;
};

// -------------------------------------------------------------------------
ctkTreeComboBoxPrivate::ctkTreeComboBoxPrivate(ctkTreeComboBox& object)
  :q_ptr(&object)
{
  this->SkipNextHide = false;
  this->RootSet = false;
  this->SendCurrentItem = false;
  this->VisibleModelColumn = -1; // all visible by default
}

// -------------------------------------------------------------------------
int ctkTreeComboBoxPrivate::computeWidthHint()const
{
  Q_Q(const ctkTreeComboBox);
  return q->view()->sizeHintForColumn(q->modelColumn());
}

// -------------------------------------------------------------------------
ctkTreeComboBox::ctkTreeComboBox(QWidget* _parent):Superclass(_parent)
  , d_ptr(new ctkTreeComboBoxPrivate(*this))
{
  QTreeView* treeView = new QTreeView(this);
  treeView->setHeaderHidden(true);
  this->setView(treeView);
  // we install the filter AFTER the QComboBox installed it.
  // so that our eventFilter will be called first
  this->view()->viewport()->installEventFilter(this);
  connect(treeView, SIGNAL(collapsed(QModelIndex)),
          this, SLOT(resizePopup()));
  connect(treeView, SIGNAL(expanded(QModelIndex)),
          this, SLOT(resizePopup()));
}

// -------------------------------------------------------------------------
ctkTreeComboBox::~ctkTreeComboBox()
{
}

// -------------------------------------------------------------------------
int ctkTreeComboBox::visibleModelColumn()const
{
  Q_D(const ctkTreeComboBox);
  return d->VisibleModelColumn;
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::setVisibleModelColumn(int index)
{
  Q_D(ctkTreeComboBox);
  d->VisibleModelColumn = index;
}

// -------------------------------------------------------------------------
bool ctkTreeComboBox::eventFilter(QObject* object, QEvent* _event)
{
  Q_D(ctkTreeComboBox);
  Q_UNUSED(object);
  bool res = false;
  d->SendCurrentItem = false;
  switch (_event->type())
    {
    default:
      break;
    case QEvent::ShortcutOverride:
      switch (static_cast<QKeyEvent*>(_event)->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Select:
          d->SendCurrentItem = true;
          break;
        default:
          break;
        }
      break;
    case QEvent::MouseButtonRelease:
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(_event); 
      QModelIndex index = this->view()->indexAt(mouseEvent->pos());
      // do we click the branch (+ or -) or the item itself ?
      if (this->view()->model()->hasChildren(index) && 
          (index.flags() & Qt::ItemIsSelectable) &&
          !this->view()->visualRect(index).contains(mouseEvent->pos()))
        {//qDebug() << "Set skip on";
        // if the branch is clicked, then we don't want to close the 
        // popup. (we don't want to select the item, just expand it.)
        // of course, all that doesn't apply with unselectable items, as
        // they won't close the popup.
        d->SkipNextHide = true;
        }

      // we want to get rid of an odd behavior. 
      // If the user highlight a selectable item and then 
      // click on the branch of an unselectable item while keeping the 
      // previous selection. The popup would be normally closed in that
      // case. We don't want that.
      if ( this->view()->model()->hasChildren(index) && 
           !(index.flags() & Qt::ItemIsSelectable) &&         
           !this->view()->visualRect(index).contains(mouseEvent->pos()))
        {//qDebug() << "eat";
        // eat the event, don't go to the QComboBox event filters.
        res = true;
        }

      d->SendCurrentItem = this->view()->rect().contains(mouseEvent->pos()) &&
        this->view()->currentIndex().isValid() &&
        (this->view()->currentIndex().flags() & Qt::ItemIsEnabled) &&
        (this->view()->currentIndex().flags() & Qt::ItemIsSelectable);
      break;
    }
  return res;
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::showPopup()
{
  Q_D(ctkTreeComboBox);
  QHeaderView* header = qobject_cast<QTreeView*>(this->view())->header();
  for (int i = 0; i < header->count(); ++i)
    {
    header->setSectionHidden(i, d->VisibleModelColumn != -1 &&
                                i != d->VisibleModelColumn);
    }
  this->QComboBox::showPopup();
  emit this->popupShow();
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::hidePopup()
{
  Q_D(ctkTreeComboBox);
  
  if (d->SkipNextHide)
    {// don't hide the popup if the selected item is a parent.
    d->SkipNextHide = false;
    //this->setCurrentIndex(-1);
    //qDebug() << "skip";
    //this->QComboBox::showPopup();
    }
  else
    {
    //QModelIndex _currentIndex = this->view()->currentIndex();
    //qDebug() << "ctkTreeComboBox::hidePopup() " << _currentIndex << " " << _currentIndex.row();
    //qDebug() << "before: " << this->currentIndex() << this->view()->currentIndex();
    this->QComboBox::hidePopup();
    //qDebug() << "after: " << this->currentIndex() << this->view()->currentIndex();
    //this->setRootModelIndex(_currentIndex.parent());
    //this->setCurrentIndex(_currentIndex.row());
    if (d->SendCurrentItem)
      {
      d->SendCurrentItem = false;
      QKeyEvent event(QEvent::ShortcutOverride, Qt::Key_Enter, Qt::NoModifier);
      QApplication::sendEvent(this->view(), &event);
      }
    emit this->popupHide();
    //qDebug() << "after2: " << this->currentIndex() << this->view()->currentIndex();
    }
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::paintEvent(QPaintEvent *p)
{
  //qDebug() << __FUNCTION__ << " " << this->currentText() << " " << this->currentIndex() ;
  //qDebug() << this->itemText(0) << this->itemText(1);
  this->QComboBox::paintEvent(p);
}

// -------------------------------------------------------------------------
QTreeView* ctkTreeComboBox::treeView()const
{
  return qobject_cast<QTreeView*>(this->view());
}
// -------------------------------------------------------------------------
void ctkTreeComboBox::resizePopup()
{
  // copied from QComboBox.cpp
  Q_D(ctkTreeComboBox);

  QStyle * const style = this->style();
  QWidget* container = qobject_cast<QWidget*>(this->view()->parent());

  QStyleOptionComboBox opt;
  this->initStyleOption(&opt);
  QRect listRect(style->subControlRect(QStyle::CC_ComboBox, &opt,
                                       QStyle::SC_ComboBoxListBoxPopup, this));
  QRect screen = QApplication::desktop()->availableGeometry(
    QApplication::desktop()->screenNumber(this));
  QPoint below = this->mapToGlobal(listRect.bottomLeft());
  int belowHeight = screen.bottom() - below.y();
  QPoint above = this->mapToGlobal(listRect.topLeft());
  int aboveHeight = above.y() - screen.y();
  bool boundToScreen = !this->window()->testAttribute(Qt::WA_DontShowOnScreen);

  const bool usePopup = style->styleHint(QStyle::SH_ComboBox_Popup, &opt, this);
    {
    int listHeight = 0;
    int count = 0;
    QStack<QModelIndex> toCheck;
    toCheck.push(this->view()->rootIndex());
#ifndef QT_NO_TREEVIEW
    QTreeView *treeView = qobject_cast<QTreeView*>(this->view());
    if (treeView && treeView->header() && !treeView->header()->isHidden())
      listHeight += treeView->header()->height();
#endif
    while (!toCheck.isEmpty())
      {
      QModelIndex parent = toCheck.pop();
      for (int i = 0; i < this->model()->rowCount(parent); ++i)
        {
        QModelIndex idx = this->model()->index(i, this->modelColumn(), parent);
        if (!idx.isValid())
          {
          continue;
          }
        listHeight += this->view()->visualRect(idx).height(); /* + container->spacing() */;
#ifndef QT_NO_TREEVIEW
        if (this->model()->hasChildren(idx) && treeView && treeView->isExpanded(idx))
          {
          toCheck.push(idx);
          }
#endif
        ++count;
        if (!usePopup && count > this->maxVisibleItems())
          {
          toCheck.clear();
          break;
          }
        }
      }
    listRect.setHeight(listHeight);
    }
      {
      // add the spacing for the grid on the top and the bottom;
      int heightMargin = 0;//2*container->spacing();

      // add the frame of the container
      int marginTop, marginBottom;
      container->getContentsMargins(0, &marginTop, 0, &marginBottom);
      heightMargin += marginTop + marginBottom;

      //add the frame of the view
      this->view()->getContentsMargins(0, &marginTop, 0, &marginBottom);
      //marginTop += static_cast<QAbstractScrollAreaPrivate *>(QObjectPrivate::get(this->view()))->top;
      //marginBottom += static_cast<QAbstractScrollAreaPrivate *>(QObjectPrivate::get(this->view()))->bottom;
      heightMargin += marginTop + marginBottom;

      listRect.setHeight(listRect.height() + heightMargin);
      }

      // Add space for margin at top and bottom if the style wants it.
      if (usePopup)
        {
        listRect.setHeight(listRect.height() + style->pixelMetric(QStyle::PM_MenuVMargin, &opt, this) * 2);
        }

      // Make sure the popup is wide enough to display its contents.
      if (usePopup)
        {
        const int diff = d->computeWidthHint() - this->width();
        if (diff > 0)
          {
          listRect.setWidth(listRect.width() + diff);
          }
        }

      //we need to activate the layout to make sure the min/maximum size are set when the widget was not yet show
      container->layout()->activate();
      //takes account of the minimum/maximum size of the container
      listRect.setSize( listRect.size().expandedTo(container->minimumSize())
                        .boundedTo(container->maximumSize()));

      // make sure the widget fits on screen
      if (boundToScreen)
        {
        if (listRect.width() > screen.width() )
          {
          listRect.setWidth(screen.width());
          }
        if (this->mapToGlobal(listRect.bottomRight()).x() > screen.right())
          {
          below.setX(screen.x() + screen.width() - listRect.width());
          above.setX(screen.x() + screen.width() - listRect.width());
          }
        if (this->mapToGlobal(listRect.topLeft()).x() < screen.x() )
          {
          below.setX(screen.x());
          above.setX(screen.x());
          }
        }

      if (usePopup)
        {
        // Position horizontally.
        listRect.moveLeft(above.x());

#ifndef Q_WS_S60
        // Position vertically so the curently selected item lines up
        // with the combo box.
        const QRect currentItemRect = this->view()->visualRect(this->view()->currentIndex());
        const int offset = listRect.top() - currentItemRect.top();
        listRect.moveTop(above.y() + offset - listRect.top());
#endif

      // Clamp the listRect height and vertical position so we don't expand outside the
      // available screen geometry.This may override the vertical position, but it is more
      // important to show as much as possible of the popup.
        const int height = !boundToScreen ? listRect.height() : qMin(listRect.height(), screen.height());
#ifdef Q_WS_S60
        //popup needs to be stretched with screen minimum dimension
        listRect.setHeight(qMin(screen.height(), screen.width()));
#else
        listRect.setHeight(height);
#endif

        if (boundToScreen)
          {
          if (listRect.top() < screen.top())
            {
            listRect.moveTop(screen.top());
            }
          if (listRect.bottom() > screen.bottom())
            {
            listRect.moveBottom(screen.bottom());
            }
          }
#ifdef Q_WS_S60
        if (screen.width() < screen.height())
          {
          // in portait, menu should be positioned above softkeys
          listRect.moveBottom(screen.bottom());
          }
        else
          {
          TRect staConTopRect = TRect();
          AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EStaconTop, staConTopRect);
          listRect.setWidth(listRect.height());
          //by default popup is centered on screen in landscape
          listRect.moveCenter(screen.center());
          if (staConTopRect.IsEmpty())
            {
            // landscape without stacon, menu should be at the right
            (opt.direction == Qt::LeftToRight) ? listRect.setRight(screen.right()) :
              listRect.setLeft(screen.left());
            }
          }
#endif
        }
      else if (!boundToScreen || listRect.height() <= belowHeight)
        {
        listRect.moveTopLeft(below);
        }
      else if (listRect.height() <= aboveHeight)
        {
        listRect.moveBottomLeft(above);
        }
      else if (belowHeight >= aboveHeight)
        {
        listRect.setHeight(belowHeight);
        listRect.moveTopLeft(below);
        }
      else
        {
        listRect.setHeight(aboveHeight);
        listRect.moveBottomLeft(above);
        }

#if QT_VERSION < QT_VERSION_CHECK(5,0,0) && !defined QT_NO_IM
      if (QInputContext *qic = this->inputContext())
        {
        qic->reset();
        }
#endif
      QScrollBar *sb = this->view()->horizontalScrollBar();
      Qt::ScrollBarPolicy policy = this->view()->horizontalScrollBarPolicy();
      bool needHorizontalScrollBar =
        (policy == Qt::ScrollBarAsNeeded || policy == Qt::ScrollBarAlwaysOn)
        && sb->minimum() < sb->maximum();
      if (needHorizontalScrollBar)
        {
        listRect.adjust(0, 0, 0, sb->height());
        }
      container->setGeometry(listRect);
}
