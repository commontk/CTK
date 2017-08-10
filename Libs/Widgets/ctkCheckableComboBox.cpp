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
#include <QAbstractItemView>
#include <QDebug>
#include <QDesktopWidget>
#include <QItemDelegate>
#include <QLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QToolBar>

// CTK includes
#include "ctkCheckableComboBox.h"
#include <ctkCheckableModelHelper.h>

// Similar to QComboBoxDelegate
class ctkComboBoxDelegate : public QItemDelegate
{
public:
    ctkComboBoxDelegate(QObject *parent, QComboBox *cmb)
      : QItemDelegate(parent), ComboBox(cmb)
    {}

    static bool isSeparator(const QModelIndex &index)
    {
      return index.data(Qt::AccessibleDescriptionRole).toString() == QLatin1String("separator");
    }
    static void setSeparator(QAbstractItemModel *model, const QModelIndex &index)
    {
      model->setData(index, QString::fromLatin1("separator"), Qt::AccessibleDescriptionRole);
      if (QStandardItemModel *m = qobject_cast<QStandardItemModel*>(model))
        {
        if (QStandardItem *item = m->itemFromIndex(index))
          {
          item->setFlags(item->flags() & ~(Qt::ItemIsSelectable|Qt::ItemIsEnabled));
          }
        }
    }

protected:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
      if (isSeparator(index))
        {
        QRect rect = option.rect;

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
        if (const QStyleOptionViewItemV3 *v3 = qstyleoption_cast<const QStyleOptionViewItemV3*>(&option))
          {
          if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView*>(v3->widget))
            {
            rect.setWidth(view->viewport()->width());
            }
          }
#else
        if (const QAbstractItemView *view = qobject_cast<const QAbstractItemView*>(option.widget))
          {
          rect.setWidth(view->viewport()->width());
          }
#endif
        QStyleOption opt;
        opt.rect = rect;
        this->ComboBox->style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, painter, this->ComboBox);
        }
      else
        {
        QItemDelegate::paint(painter, option, index);
        }
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
    {
      if (isSeparator(index))
        {
        int pm = this->ComboBox->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this->ComboBox);
        return QSize(pm, pm);
        }
      return this->QItemDelegate::sizeHint(option, index);
    }
private:
    QComboBox* ComboBox;
};

//-----------------------------------------------------------------------------
class ctkCheckableComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckableComboBox);
protected:
  ctkCheckableComboBox* const q_ptr;
  QModelIndexList checkedIndexes()const;
  QModelIndexList uncheckedIndexes()const;

public:
  ctkCheckableComboBoxPrivate(ctkCheckableComboBox& object);
  void init();

  QModelIndexList cachedCheckedIndexes()const;
  void updateCheckedList();

  ctkCheckableModelHelper* CheckableModelHelper;
  bool MouseButtonPressed;

private:
  QModelIndexList persistentIndexesToModelIndexes(
    const QList<QPersistentModelIndex>& persistentModels)const;
  QList<QPersistentModelIndex> modelIndexesToPersistentIndexes(
    const QModelIndexList& modelIndexes)const;

  mutable QList<QPersistentModelIndex> CheckedList;
};

//-----------------------------------------------------------------------------
ctkCheckableComboBoxPrivate::ctkCheckableComboBoxPrivate(ctkCheckableComboBox& object)
  : q_ptr(&object)
{
  this->CheckableModelHelper = 0;
  this->MouseButtonPressed = false;
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBoxPrivate::init()
{
  Q_Q(ctkCheckableComboBox);
  this->CheckableModelHelper = new ctkCheckableModelHelper(Qt::Horizontal, q);
  this->CheckableModelHelper->setForceCheckability(true);
  
  q->setCheckableModel(q->model());
  q->view()->installEventFilter(q);
  q->view()->viewport()->installEventFilter(q);
  // QCleanLooksStyle uses a delegate that doesn't show the checkboxes in the
  // popup list.
  q->setItemDelegate(new ctkComboBoxDelegate(q->view(), q));
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBoxPrivate::updateCheckedList()
{
  Q_Q(ctkCheckableComboBox);
  QList<QPersistentModelIndex> newCheckedPersistentList =
    this->modelIndexesToPersistentIndexes(this->checkedIndexes());
  if (newCheckedPersistentList == this->CheckedList)
    {
    return;
    }
  this->CheckedList = newCheckedPersistentList;
  emit q->checkedIndexesChanged();
}

//-----------------------------------------------------------------------------
QList<QPersistentModelIndex> ctkCheckableComboBoxPrivate
::modelIndexesToPersistentIndexes(const QModelIndexList& indexes)const
{
  QList<QPersistentModelIndex> res;
  foreach(const QModelIndex& index, indexes)
    {
    QPersistentModelIndex persistent(index);
    if (persistent.isValid())
      {
      res << persistent;
      }
    }
  return res;
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBoxPrivate
::persistentIndexesToModelIndexes(
  const QList<QPersistentModelIndex>& indexes)const
{
  QModelIndexList res;
  foreach(const QPersistentModelIndex& index, indexes)
    {
    if (index.isValid())
      {
      res << index;
      }
    }
  return res;
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBoxPrivate::cachedCheckedIndexes()const
{
  return this->persistentIndexesToModelIndexes(this->CheckedList);
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBoxPrivate::checkedIndexes()const
{
  Q_Q(const ctkCheckableComboBox);
  QModelIndex startIndex = q->model()->index(0,0, q->rootModelIndex());
  return q->model()->match(
    startIndex, Qt::CheckStateRole,
    static_cast<int>(Qt::Checked), -1, Qt::MatchRecursive);
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBoxPrivate::uncheckedIndexes()const
{
  Q_Q(const ctkCheckableComboBox);
  QModelIndex startIndex = q->model()->index(0,0, q->rootModelIndex());
  return q->model()->match(
    startIndex, Qt::CheckStateRole,
    static_cast<int>(Qt::Unchecked), -1, Qt::MatchRecursive);
}

//-----------------------------------------------------------------------------
ctkCheckableComboBox::ctkCheckableComboBox(QWidget* parentWidget)
  : QComboBox(parentWidget)
  , d_ptr(new ctkCheckableComboBoxPrivate(*this))
{
  Q_D(ctkCheckableComboBox);
  d->init();
}

//-----------------------------------------------------------------------------
ctkCheckableComboBox::~ctkCheckableComboBox()
{
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::eventFilter(QObject *o, QEvent *e)
{
  Q_D(ctkCheckableComboBox);
  switch (e->type())
    {
    case QEvent::MouseButtonPress:
      {
      if (this->view()->isVisible())
        {
        d->MouseButtonPressed = true;
        }
      break;
      }
    case QEvent::MouseButtonRelease:
      {
      QMouseEvent *m = static_cast<QMouseEvent *>(e);
      if (this->view()->isVisible() && 
          this->view()->rect().contains(m->pos()) &&
          this->view()->currentIndex().isValid()
          //&& !blockMouseReleaseTimer.isActive()
          && (this->view()->currentIndex().flags() & Qt::ItemIsEnabled)
          && (this->view()->currentIndex().flags() & Qt::ItemIsSelectable))
        {
        // The signal to open the menu is fired when the mouse button is
        // pressed, we don't want to toggle the item under the mouse cursor
        // when the button used to open the popup is released.
        if (d->MouseButtonPressed)
          {
          // make the item current, it will then call QComboBox::update (and
          // repaint) when the current index data is changed (checkstate
          // toggled fires dataChanged signal which is observed).
          this->setCurrentIndex(this->view()->currentIndex().row());
          d->CheckableModelHelper->toggleCheckState(this->view()->currentIndex());
          }
        d->MouseButtonPressed = false;
        return true;
        }
      d->MouseButtonPressed = false;
      break;
      } 
    default:
        break;
    }
  return this->QComboBox::eventFilter(o, e);
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::setCheckableModel(QAbstractItemModel* newModel)
{
  Q_D(ctkCheckableComboBox);
  this->disconnect(this->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                   this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
  if (newModel != this->model())
    {
    this->setModel(newModel);
    }
  this->connect(this->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
  d->CheckableModelHelper->setModel(newModel);
  d->updateCheckedList();
}

//-----------------------------------------------------------------------------
QAbstractItemModel* ctkCheckableComboBox::checkableModel()const
{
  return this->model();
}

//-----------------------------------------------------------------------------
QModelIndexList ctkCheckableComboBox::checkedIndexes()const
{
  Q_D(const ctkCheckableComboBox);
  return d->cachedCheckedIndexes();
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::allChecked()const
{
  Q_D(const ctkCheckableComboBox);
  return d->uncheckedIndexes().count() == 0;
}

//-----------------------------------------------------------------------------
bool ctkCheckableComboBox::noneChecked()const
{
  Q_D(const ctkCheckableComboBox);
  return d->cachedCheckedIndexes().count() == 0;
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::setCheckState(const QModelIndex& index, Qt::CheckState check)
{
  Q_D(ctkCheckableComboBox);
  return d->CheckableModelHelper->setCheckState(index, check);
}

//-----------------------------------------------------------------------------
Qt::CheckState ctkCheckableComboBox::checkState(const QModelIndex& index)const
{
  Q_D(const ctkCheckableComboBox);
  return d->CheckableModelHelper->checkState(index);
}

//-----------------------------------------------------------------------------
ctkCheckableModelHelper* ctkCheckableComboBox::checkableModelHelper()const
{
  Q_D(const ctkCheckableComboBox);
  return d->CheckableModelHelper;
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::onDataChanged(const QModelIndex& start, const QModelIndex& end)
{
  Q_D(ctkCheckableComboBox);
  Q_UNUSED(start);
  Q_UNUSED(end);
  d->updateCheckedList();
}

//-----------------------------------------------------------------------------
void ctkCheckableComboBox::paintEvent(QPaintEvent *)
{
  Q_D(ctkCheckableComboBox);

  QStylePainter painter(this);
  painter.setPen(palette().color(QPalette::Text));

  // draw the combobox frame, focusrect and selected etc.
  QStyleOptionComboBox opt;
  this->initStyleOption(&opt);

  if (this->allChecked())
    {
    opt.currentText = "All";
    opt.currentIcon = QIcon();
    }
  else if (this->noneChecked())
    {
    opt.currentText = "None";
    opt.currentIcon = QIcon();
    }
  else
    {
    //search the checked items
    QModelIndexList indexes = d->cachedCheckedIndexes();
    if (indexes.count() == 1)
      {
      opt.currentText = this->model()->data(indexes[0], Qt::DisplayRole).toString();
      opt.currentIcon = qvariant_cast<QIcon>(this->model()->data(indexes[0], Qt::DecorationRole));
      }
    else
      {
      QStringList indexesText;
      foreach(QModelIndex checkedIndex, indexes)
        {
        indexesText << this->model()->data(checkedIndex, Qt::DisplayRole).toString();
        }
      opt.currentText = indexesText.join(", ");
      opt.currentIcon = QIcon();
      }
    }
  painter.drawComplexControl(QStyle::CC_ComboBox, opt);

  // draw the icon and text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}
