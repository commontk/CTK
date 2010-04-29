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
};

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(QWidget* _parent) : Superclass(4, 4, _parent)
{
  CTK_INIT_PRIVATE(ctkMatrixWidget);

  // Set Read-only
  this->setEditTriggers(ctkMatrixWidget::NoEditTriggers);

  // Hide headers
  this->verticalHeader()->hide();
  this->horizontalHeader()->hide();

  // Disable scrollBars
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Define prototype item
  QTableWidgetItem* _item = new QTableWidgetItem();
  _item->setData(Qt::DisplayRole, QVariant(0.0));
  _item->setTextAlignment(Qt::AlignCenter);

  // The table takes ownership of the prototype.
  this->setItemPrototype(_item);

  // Initialize
  this->reset();
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::minimumSizeHint () const
{
  return QSize(this->columnCount() * 25, this->rowCount() * 25);
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::sizeHint () const
{
  return this->minimumSizeHint();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::resizeEvent(QResizeEvent * _event)
{
  this->Superclass::resizeEvent(_event);
  this->adjustRowsColumnsSize(_event->size().width(), _event->size().height());
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::adjustRowsColumnsSize(int _width, int _height)
{
  int colwidth = _width / this->columnCount();
  int lastColwidth = colwidth + (_width - colwidth * this->columnCount());
  //qDebug() << "width:" << width << ",col-width:" << colwidth;
  for (int j=0; j < this->columnCount(); j++)
    {
    bool lastColumn = (j==(this->columnCount()-1));
    this->setColumnWidth(j, lastColumn ? lastColwidth : colwidth);
    }

  int rowheight = _height / this->rowCount();
  int lastRowheight = rowheight + (_height - rowheight * this->rowCount());
  //qDebug() << "height:" << height << ", row-height:" << rowheight;
  for (int i=0; i < this->rowCount(); i++)
    {
    bool lastRow = (i==(this->rowCount()-1));
    this->setRowHeight(i, lastRow ? lastRowheight : rowheight);
    }
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
double ctkMatrixWidget::value(int i, int j)
{
  if (i<0 || i>=(this->rowCount()) || j<0 || j>=this->columnCount()) { return 0; }

  return this->item(i, j)->data(Qt::DisplayRole).toDouble();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setValue(int i, int j, double _value)
{
  if (i<0 || i>=(this->rowCount()) || j<0 || j>=this->columnCount()) { return; }

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
