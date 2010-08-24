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
#include <QDebug>

//-----------------------------------------------------------------------------
class ctkMatrixWidgetPrivate: public ctkPrivate<ctkMatrixWidget>
{
public:
  void init();
};

//-----------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::init()
{
  CTK_P(ctkMatrixWidget);
  // Set Read-only
  p->setEditTriggers(ctkMatrixWidget::NoEditTriggers);

  // Hide headers
  p->verticalHeader()->hide();
  p->horizontalHeader()->hide();

  // Disable scrollBars
  p->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  p->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Don't expand for no reason
  p->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  // Disable the frame by default
  p->setFrameStyle(QFrame::NoFrame);

  // Define prototype item
  QTableWidgetItem* _item = new QTableWidgetItem();
  _item->setData(Qt::DisplayRole, QVariant(0.0));
  _item->setTextAlignment(Qt::AlignCenter);

  // The table takes ownership of the prototype.
  p->setItemPrototype(_item);

  // Initialize
  p->reset();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(QWidget* _parent) : Superclass(4, 4, _parent)
{
  CTK_INIT_PRIVATE(ctkMatrixWidget);
  CTK_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(int rows, int columns, QWidget* _parent)
  : Superclass(rows, columns, _parent)
{
  CTK_INIT_PRIVATE(ctkMatrixWidget);
  CTK_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::minimumSizeHint () const
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
QSize ctkMatrixWidget::sizeHint () const
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
  // Initialize 4x4 matrix
  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      this->setItem(i, j, this->itemPrototype()->clone());
      if (i == j)
        {
        this->setValue(i, j, 1);
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
  Q_ASSERT( i>=0 && i<this->rowCount() &&
            j>=0 && j<this->columnCount());

  this->item(i, j)->setData(Qt::DisplayRole, QVariant(_value));
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setVector(const QVector<double> & vector)
{
  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      this->item(i,j)->setData(Qt::DisplayRole, QVariant(vector.at(i * this->columnCount() + j)));
      }
    }
}
