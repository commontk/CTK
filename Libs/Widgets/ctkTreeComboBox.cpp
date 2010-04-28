/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
=========================================================================*/

// Qt includes
#include <QEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QTreeView>
#include <QDebug>

// CTK includes
#include "ctkTreeComboBox.h"

// -------------------------------------------------------------------------
class ctkTreeComboBoxPrivate: public ctkPrivate<ctkTreeComboBox>
{
public:
  bool SkipNextHide;
  bool ResetPopupSize;
  
  void init()
  {
    this->SkipNextHide = false;
    this->ResetPopupSize = false;
  }
};

// -------------------------------------------------------------------------
ctkTreeComboBox::ctkTreeComboBox(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkTreeComboBox);
  CTK_D(ctkTreeComboBox);
  
  d->init();
  QTreeView* treeView = new QTreeView(this);
  treeView->setHeaderHidden(true);
  this->setView(treeView);
  // we install the filter AFTER the QComboBox installed it.
  // so that our eventFilter will be called first
  this->view()->viewport()->installEventFilter(this);
  connect(treeView, SIGNAL(collapsed(const QModelIndex&)),
          this, SLOT(onCollapsed(const QModelIndex&)));
  connect(treeView, SIGNAL(expanded(const QModelIndex&)),
          this, SLOT(onExpanded(const QModelIndex&)));
}

// -------------------------------------------------------------------------
bool ctkTreeComboBox::eventFilter(QObject* object, QEvent* _event)
{
  CTK_D(ctkTreeComboBox);
  
  bool res = false;
  if (_event->type() == QEvent::MouseButtonRelease && 
      object == this->view()->viewport())
    {
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

    if (d->ResetPopupSize)
      {
      d->ResetPopupSize = false;
      //this->QComboBox::showPopup();
      }
    }
  return res;
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::showPopup()
{
  this->setRootModelIndex(QModelIndex());
  this->QComboBox::showPopup();
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::hidePopup()
{
  CTK_D(ctkTreeComboBox);
  
  if (d->SkipNextHide)
    {// don't hide the popup if the selected item is a parent.
    d->SkipNextHide = false;
    //this->setCurrentIndex(-1);
    //qDebug() << "skip";
    //this->QComboBox::showPopup();
    }
  else
    {
    QModelIndex _currentIndex = this->view()->currentIndex();
    //qDebug() << "ctkTreeComboBox::hidePopup() " << _currentIndex << " " << _currentIndex.row();
    //qDebug() << "before: " << this->currentIndex() << this->view()->currentIndex();
    this->QComboBox::hidePopup();
    //qDebug() << "after: " << this->currentIndex() << this->view()->currentIndex();
    this->setRootModelIndex(_currentIndex.parent());
    this->setCurrentIndex(_currentIndex.row());
    //qDebug() << "after2: " << this->currentIndex() << this->view()->currentIndex();
    }
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::onCollapsed(const QModelIndex& index)
{
  CTK_D(ctkTreeComboBox);
  
  if (this->view()->currentIndex().parent() == index)
    {
    // in the case the current item is a child of the collapsed/expanded item.
    // we don't want to resize the popup as it would undo the collapsed item.
    return;
    }
  d->ResetPopupSize = true;
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::onExpanded(const QModelIndex& /*index*/)
{
  ctk_d()->ResetPopupSize = true;
}

// -------------------------------------------------------------------------
void ctkTreeComboBox::paintEvent(QPaintEvent *p)
{
  //qDebug() << __FUNCTION__ << " " << this->currentText() << " " << this->currentIndex() ;
  //qDebug() << this->itemText(0) << this->itemText(1);
  this->QComboBox::paintEvent(p);
}
