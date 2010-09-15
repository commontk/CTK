/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

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

// CTK includes
#include "ctkMatrixWidget.h"

// Qt includes
#include <Qt>
#include <QHeaderView>
#include <QVariant>
#include <QTableWidgetItem>
#include <QResizeEvent>
#include <QDoubleSpinBox>
#include <QItemEditorFactory>
#include <QStyledItemDelegate>
#include <QDebug>

//-----------------------------------------------------------------------------
// Custom item editors

namespace
{
//-----------------------------------------------------------------------------
  class CustomDoubleSpinBox : public QDoubleSpinBox
  {
  public:
    CustomDoubleSpinBox(QWidget * newParent):QDoubleSpinBox(newParent)
    {
      // We know that the parentWidget of newParent will be a ctkMatrixWidget because this object is
      // created by the QItemEditorFactory
      ctkMatrixWidget* matrixWidget = qobject_cast<ctkMatrixWidget*>(newParent->parentWidget());
      Q_ASSERT(matrixWidget);
      
      this->setMinimum(matrixWidget->minimum());
      this->setMaximum(matrixWidget->maximum());
      this->setDecimals(matrixWidget->decimals());
      this->setSingleStep(matrixWidget->singleStep());
    }
  };
}

//-----------------------------------------------------------------------------
class ctkMatrixWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkMatrixWidget);
protected:
  ctkMatrixWidget* const q_ptr;
public:
  ctkMatrixWidgetPrivate(ctkMatrixWidget& object);

  void init();
  void validateElements();

  // Parameters for the spinbox used to change the value of matrix elements
  double Minimum;
  double Maximum;
  int Decimals;
  double SingleStep;
};

//-----------------------------------------------------------------------------
ctkMatrixWidgetPrivate::ctkMatrixWidgetPrivate(ctkMatrixWidget& object)
  :q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::init()
{
  Q_Q(ctkMatrixWidget);
  // Set Read-only
  q->setEditable(false);

  // Set parameters for the spinbox
  this->Minimum = -100;
  this->Maximum = 100;
  this->Decimals = 2;
  this->SingleStep = 0.01;

  // Hide headers
  q->verticalHeader()->hide();
  q->horizontalHeader()->hide();

  // Disable scrollBars
  q->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  q->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Don't expand for no reason
  q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  // Disable the frame by default
  q->setFrameStyle(QFrame::NoFrame);

  // Register custom editors
  QItemEditorFactory *editorFactory = new QItemEditorFactory;
  editorFactory->registerEditor(QVariant::Double, new QStandardItemEditorCreator<CustomDoubleSpinBox>);

  QStyledItemDelegate* defaultItemDelegate =
    qobject_cast<QStyledItemDelegate*>(q->itemDelegate());
  Q_ASSERT(defaultItemDelegate);
  defaultItemDelegate->setItemEditorFactory(editorFactory);

  // Define prototype item
  QTableWidgetItem* _item = new QTableWidgetItem();
  _item->setData(Qt::DisplayRole, QVariant(0.0));
  _item->setTextAlignment(Qt::AlignCenter);

  // The table takes ownership of the prototype.
  q->setItemPrototype(_item);

  // Initialize
  q->reset();
}

// --------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::validateElements()
{
  Q_Q(ctkMatrixWidget);
  for (int i=0; i < q->rowCount(); i++)
    {
    for (int j=0; j < q->columnCount(); j++)
      {
      double value = q->item(i, j)->data(Qt::DisplayRole).toDouble();
      if (value < this->Minimum)
        {
        q->item(i,j)->setData(Qt::DisplayRole, QVariant(this->Minimum));
        }
      if (value > this->Maximum)
        {
        q->item(i,j)->setData(Qt::DisplayRole, QVariant(this->Maximum));
        }
      }
    }
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(QWidget* _parent)
  :Superclass(4, 4, _parent)
  ,d_ptr(new ctkMatrixWidgetPrivate(*this))
{
  Q_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(int rows, int columns, QWidget* _parent)
  :Superclass(rows, columns, _parent)
  ,d_ptr(new ctkMatrixWidgetPrivate(*this))
{
  Q_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(int rows, int columns, ctkMatrixWidgetPrivate& pvt,
                                 QWidget* _parent)
  :Superclass(rows, columns, _parent)
  ,d_ptr(&pvt)
{
  Q_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::~ctkMatrixWidget()
{
}

// --------------------------------------------------------------------------
bool ctkMatrixWidget::editable()const
{
  return this->editTriggers();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setEditable(bool newEditable)
{
  if (newEditable)
    {
    this->setEditTriggers(ctkMatrixWidget::DoubleClicked);
    }
  else
    {
    this->setEditTriggers(ctkMatrixWidget::NoEditTriggers);
    }
}

// --------------------------------------------------------------------------
CTK_GET_CXX(ctkMatrixWidget, double, minimum, Minimum);
CTK_GET_CXX(ctkMatrixWidget, double, maximum, Maximum);
CTK_GET_CXX(ctkMatrixWidget, double, singleStep, SingleStep);
CTK_SET_CXX(ctkMatrixWidget, double, setSingleStep, SingleStep);
CTK_GET_CXX(ctkMatrixWidget, int, decimals, Decimals);
CTK_SET_CXX(ctkMatrixWidget, int, setDecimals, Decimals);

// --------------------------------------------------------------------------
void ctkMatrixWidget::setMinimum(double newMinimum)
{
  Q_D(ctkMatrixWidget);
  d->Minimum = newMinimum;
  d->validateElements();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setMaximum(double newMaximum)
{
  Q_D(ctkMatrixWidget);
  d->Maximum = newMaximum;
  d->validateElements();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setRange(double newMinimum, double newMaximum)
{
  Q_D(ctkMatrixWidget);
  d->Minimum = newMinimum;
  d->Maximum = newMaximum;
  d->validateElements();
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::minimumSizeHint() const
{
  int maxWidth = this->sizeHintForColumn(0);
  for (int j = 1; j < this->columnCount(); ++j)
    {
    maxWidth = qMax(maxWidth, this->sizeHintForColumn(j));
    }
  int maxHeight = this->sizeHintForRow(0);
  for (int i = 1; i < this->rowCount(); ++i)
    {
    maxHeight = qMax(maxHeight, this->sizeHintForRow(i));
    }
  return QSize(maxWidth*this->columnCount(), maxHeight*this->rowCount());
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::sizeHint() const
{
  return this->minimumSizeHint();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::updateGeometries()
{
  QSize viewportSize = this->viewport()->size();
  // columns
  const int ccount = this->columnCount();
  int colWidth = viewportSize.width() / ccount;
  int lastColWidth = colWidth
    + (viewportSize.width() - colWidth * ccount);
  for (int j=0; j < ccount; j++)
    {
    bool lastColumn = (j==(ccount-1));
    int newWidth = lastColumn ? lastColWidth : colWidth;
    this->setColumnWidth(j, newWidth);
    Q_ASSERT(this->columnWidth(j) == newWidth);
    }
  // rows
  const int rcount = this->rowCount();
  int rowHeight = viewportSize.height() / rcount;
  int lastRowHeight = rowHeight + (viewportSize.height() - rowHeight * rcount);
  for (int i=0; i < rcount; i++)
    {
    bool lastRow = (i==(rcount-1));
    int newHeight = lastRow ? lastRowHeight : rowHeight;
    this->setRowHeight(i, newHeight);
    Q_ASSERT(this->rowHeight(i) == newHeight);
    }

  this->Superclass::updateGeometries();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::reset()
{
  Q_D(ctkMatrixWidget);
  // Initialize 4x4 matrix
  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      this->setItem(i, j, this->itemPrototype()->clone());
      if (i == j)
        {
        if (d->Maximum < 1.0)
          {
          this->setValue(i, j, d->Maximum);
          }
        else if (d->Minimum > 1.0)
          {
          this->setValue(i, j, d->Minimum);
          }
        else
          {
          this->setValue(i, j, 1.0);
          }
        }
      }
    }
}

// --------------------------------------------------------------------------
double ctkMatrixWidget::value(int i, int j)const
{
  Q_ASSERT( i>=0 && i<this->rowCount() &&
            j>=0 && j<this->columnCount());

  return this->item(i, j)->data(Qt::DisplayRole).toDouble();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setValue(int i, int j, double _value)
{
  Q_D(ctkMatrixWidget);
  Q_ASSERT( i>=0 && i<this->rowCount() &&
            j>=0 && j<this->columnCount());

  if (_value >= d->Minimum && _value <= d->Maximum)
    {
    this->item(i, j)->setData(Qt::DisplayRole, QVariant(_value));
    }
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setVector(const QVector<double> & vector)
{
  Q_D(ctkMatrixWidget);
  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      double value = vector.at(i * this->columnCount() + j);
      if  (value >= d->Minimum && value <= d->Maximum)
        {
        this->item(i,j)->setData(Qt::DisplayRole, QVariant(value));
        }
      }
    }
}
