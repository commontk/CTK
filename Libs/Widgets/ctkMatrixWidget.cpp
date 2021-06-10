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

// CTK includes
#include "ctkMatrixWidget.h"
#include "ctkDoubleSpinBox.h"
#include "ctkUtils.h"

// Qt includes
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QItemEditorFactory>
#include <QResizeEvent>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVariant>
#include <Qt>

//-----------------------------------------------------------------------------
// Custom item editors

namespace
{
//-----------------------------------------------------------------------------
  class ctkMatrixDoubleSpinBox : public ctkDoubleSpinBox
  {
  public:
    ctkMatrixDoubleSpinBox(QWidget * parentWidget)
      : ctkDoubleSpinBox(parentWidget)
    {
      // We know that the parentWidget() of parentWidget will be a
      // ctkMatrixWidget because this object is
      // created by the QItemEditorFactory
      ctkMatrixWidget* matrixWidget =
        qobject_cast<ctkMatrixWidget*>(parentWidget->parentWidget()->parent());
      Q_ASSERT(matrixWidget);
      this->setMinimum(matrixWidget->minimum());
      this->setMaximum(matrixWidget->maximum());
      this->setDecimals(matrixWidget->decimals());
      this->setDecimalsOption(matrixWidget->decimalsOption());
      this->setSingleStep(matrixWidget->singleStep());

      this->connect(this, SIGNAL(decimalsChanged(int)),
        matrixWidget, SLOT(setDecimals(int)));
    }
  };

//-----------------------------------------------------------------------------
// Reimplemented to display the numbers with the matrix decimals.
class ctkMatrixItemDelegate : public QStyledItemDelegate
{
public:
  ctkMatrixItemDelegate(ctkMatrixWidget* matrixWidget)
    : QStyledItemDelegate(matrixWidget)
  {
  }
  virtual QString	displayText ( const QVariant & value, const QLocale & locale ) const
  {
    ctkMatrixWidget* matrix = qobject_cast<ctkMatrixWidget*>(this->parent());
    Q_ASSERT(matrix);
    switch(value.type())
      {
      case QVariant::Double:
        return locale.toString(value.toDouble(), 'f', matrix->decimals());
      default:
        return this->QStyledItemDelegate::displayText(value, locale);
      }
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
  ctkMatrixWidgetPrivate(ctkMatrixWidget& object, int rows = 4, int columns = 4);

  void init();
  void validateItems();
  void updateGeometries();
  void setIdentityItem(int i, int j);

  QTableWidget* Table;

  // Parameters for the spinbox used to change the value of matrix elements
  double Minimum;
  double Maximum;
  int    Decimals;
  ctkDoubleSpinBox::DecimalsOptions DecimalsOption;
  double SingleStep;
};

//-----------------------------------------------------------------------------
ctkMatrixWidgetPrivate::ctkMatrixWidgetPrivate(ctkMatrixWidget& object, int rows, int columns)
  :q_ptr(&object)
{
  this->Table = new QTableWidget(rows, columns);
}

//-----------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::init()
{
  Q_Q(ctkMatrixWidget);
  this->Table->setItemDelegate(new ctkMatrixItemDelegate(q));

  this->Table->setParent(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->Table);
  layout->setContentsMargins(0,0,0,0);
  q->setLayout(layout);

  // Set parameters for the spinbox
  // TODO: not sure [-100. 100.] is the right default range
  this->Minimum = -100;
  this->Maximum = 100;
  this->Decimals = 2;
  this->SingleStep = 0.01;

  // Don't select the items
  this->Table->setSelectionMode(QAbstractItemView::NoSelection);

  // Hide headers
  this->Table->verticalHeader()->hide();
  this->Table->horizontalHeader()->hide();

  // Disable scrollBars
  this->Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->Table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Don't expand for no reason
  q->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  // Disable the frame by default
  this->Table->setFrameStyle(QFrame::NoFrame);

  // Register custom editors
  QItemEditorFactory *editorFactory = new QItemEditorFactory;
  editorFactory->registerEditor(QVariant::Double, new QStandardItemEditorCreator<ctkMatrixDoubleSpinBox>);

  QStyledItemDelegate* defaultItemDelegate =
    qobject_cast<QStyledItemDelegate*>(this->Table->itemDelegate());
  Q_ASSERT(defaultItemDelegate);
  defaultItemDelegate->setItemEditorFactory(editorFactory);

  // Define prototype item
  QTableWidgetItem* _item = new QTableWidgetItem;
  _item->setData(Qt::DisplayRole, QVariant(0.0));
  _item->setTextAlignment(Qt::AlignCenter);

  // The table takes ownership of the prototype.
  this->Table->setItemPrototype(_item);

  QObject::connect(this->Table, SIGNAL(cellChanged(int,int)),
                   q, SIGNAL(matrixChanged()));
  /// \todo Wrap model signals to emit signals when the matrix is changed.
/// Right now you can connect to the signal:
/// matrixWidget->model()->dataChanged(...)

  // Set Read-only
  q->setEditable(true);

  // Initialize
  this->validateItems();

  this->updateGeometries();
}

// --------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::validateItems()
{
  Q_Q(ctkMatrixWidget);
  for (int i=0; i < q->rowCount(); ++i)
    {
    for (int j=0; j < q->columnCount(); ++j)
      {
      QTableWidgetItem* item = this->Table->item(i, j);
      if (!item)
        {
        this->Table->setItem(i, j , this->Table->itemPrototype()->clone());
        this->setIdentityItem(i, j);
        }
      else
        {
        double value = item->data(Qt::DisplayRole).toDouble();
        item->setData(Qt::DisplayRole,
                      qBound(this->Minimum, value, this->Maximum));
        }
      }
    }
}

// --------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::setIdentityItem(int i, int j)
{
  Q_Q(ctkMatrixWidget);
  // the item must exist first
  Q_ASSERT(this->Table->item(i, j));
  // identity matrix has 1 on the diagonal, 0 everywhere else
  double value = (i == j ? 1. : 0.);
  // set the value to the table
  q->setValue(i, j, value);
}

// --------------------------------------------------------------------------
void ctkMatrixWidgetPrivate::updateGeometries()
{
  Q_Q(ctkMatrixWidget);
  QSize viewportSize = q->size();
  // columns
  const int ccount = q->columnCount();
  int colWidth = viewportSize.width() / ccount;
  int lastColWidth = colWidth
    + (viewportSize.width() - colWidth * ccount);
  for (int j=0; j < ccount; j++)
    {
    bool lastColumn = (j==(ccount-1));
    int newWidth = lastColumn ? lastColWidth : colWidth;
    this->Table->setColumnWidth(j, newWidth);
    }
  // rows
  const int rcount = q->rowCount();
  int rowHeight = viewportSize.height() / rcount;
  int lastRowHeight = rowHeight + (viewportSize.height() - rowHeight * rcount);
  for (int i=0; i < rcount; i++)
    {
    bool lastRow = (i==(rcount-1));
    int newHeight = lastRow ? lastRowHeight : rowHeight;
    this->Table->setRowHeight(i, newHeight);
    }
  this->Table->updateGeometry();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(QWidget* _parent)
  :Superclass(_parent)
  ,d_ptr(new ctkMatrixWidgetPrivate(*this))
{
  Q_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(int rows, int columns, QWidget* _parent)
  : QWidget(_parent)
  , d_ptr(new ctkMatrixWidgetPrivate(*this, rows, columns))
{
  Q_D(ctkMatrixWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkMatrixWidget::ctkMatrixWidget(ctkMatrixWidgetPrivate& pvt,
                                 QWidget* _parent)
  : Superclass(_parent)
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
int ctkMatrixWidget::columnCount()const
{
  Q_D(const ctkMatrixWidget);
  return d->Table->columnCount();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setColumnCount(int rc)
{
  Q_D(ctkMatrixWidget);
  d->Table->setColumnCount(rc);
  d->validateItems();
  d->updateGeometries();
}

// --------------------------------------------------------------------------
int ctkMatrixWidget::rowCount()const
{
  Q_D(const ctkMatrixWidget);
  return d->Table->rowCount();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setRowCount(int rc)
{
  Q_D(ctkMatrixWidget);
  d->Table->setRowCount(rc);
  d->validateItems();
  d->updateGeometries();
}

// --------------------------------------------------------------------------
bool ctkMatrixWidget::isEditable()const
{
  Q_D(const ctkMatrixWidget);
  return d->Table->editTriggers();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setEditable(bool newEditable)
{
  Q_D(ctkMatrixWidget);
  d->Table->setEditTriggers(
    newEditable ? QTableWidget::DoubleClicked : QTableWidget::NoEditTriggers);
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkMatrixWidget, double, minimum, Minimum);
CTK_GET_CPP(ctkMatrixWidget, double, maximum, Maximum);
CTK_GET_CPP(ctkMatrixWidget, double, singleStep, SingleStep);
CTK_SET_CPP(ctkMatrixWidget, double, setSingleStep, SingleStep);
CTK_GET_CPP(ctkMatrixWidget, int, decimals, Decimals);

// --------------------------------------------------------------------------
void ctkMatrixWidget::setMinimum(double newMinimum)
{
  Q_D(ctkMatrixWidget);
  d->Minimum = newMinimum;
  d->Maximum = qMax(newMinimum, d->Maximum);
  d->validateItems();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setMaximum(double newMaximum)
{
  Q_D(ctkMatrixWidget);
  d->Minimum = qMin(d->Minimum, newMaximum);
  d->Maximum = newMaximum;
  d->validateItems();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setRange(double newMinimum, double newMaximum)
{
  Q_D(ctkMatrixWidget);
  d->Minimum = qMin(newMinimum, newMaximum);
  d->Maximum = qMax(newMinimum, newMaximum);
  d->validateItems();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setDecimals(int decimals)
{
  Q_D(ctkMatrixWidget);
  if (d->Decimals == decimals)
    {
    return;
    }
  d->Decimals = qMax(0, decimals);
  this->update();
  this->emit decimalsChanged(d->Decimals);
}

// --------------------------------------------------------------------------
ctkDoubleSpinBox::DecimalsOptions ctkMatrixWidget::decimalsOption()const
{
  Q_D(const ctkMatrixWidget);
  return d->DecimalsOption;
}

// --------------------------------------------------------------------------
void ctkMatrixWidget
::setDecimalsOption(ctkDoubleSpinBox::DecimalsOptions newDecimalsOption)
{
  Q_D(ctkMatrixWidget);
  d->DecimalsOption = newDecimalsOption;
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::minimumSizeHint() const
{
  Q_D(const ctkMatrixWidget);
  int maxWidth = d->Table->horizontalHeader()->sectionSizeHint(0);
  for (int j = 1; j < this->columnCount(); ++j)
    {
    maxWidth = qMax(maxWidth, d->Table->horizontalHeader()->sectionSizeHint(j));
    }
  int maxHeight = d->Table->verticalHeader()->sectionSizeHint(0);
  for (int i = 1; i < this->rowCount(); ++i)
    {
    maxHeight = qMax(maxHeight, d->Table->verticalHeader()->sectionSizeHint(i));
    }
  return QSize(maxWidth*this->columnCount(), maxHeight*this->rowCount());
}

// --------------------------------------------------------------------------
QSize ctkMatrixWidget::sizeHint() const
{
  return this->minimumSizeHint();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkMatrixWidget);
  this->Superclass::resizeEvent(event);
  d->updateGeometries();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::identity()
{
  Q_D(ctkMatrixWidget);
  // Initialize 4x4 matrix
  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      d->setIdentityItem(i,j);
      }
    }
}

// --------------------------------------------------------------------------
double ctkMatrixWidget::value(int i, int j)const
{
  Q_D(const ctkMatrixWidget);
  Q_ASSERT( i>=0 && i<this->rowCount() &&
            j>=0 && j<this->columnCount());

  return d->Table->item(i, j)->data(Qt::DisplayRole).toDouble();
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setValue(int i, int j, double newValue)
{
  Q_D(ctkMatrixWidget);
  Q_ASSERT( i>=0 && i<this->rowCount() &&
            j>=0 && j<this->columnCount());

  newValue = qBound(d->Minimum, newValue, d->Maximum);
  d->Table->item(i, j)->setData(Qt::DisplayRole, QVariant(newValue));
}

// --------------------------------------------------------------------------
QVector<double> ctkMatrixWidget::values()const
{
  QVector<double> values;

  for (int i=0; i < this->rowCount(); i++)
    {
    for (int j=0; j < this->columnCount(); j++)
      {
      values.push_back(this->value(i,j));
      }
    }

  return values;
}

// --------------------------------------------------------------------------
void ctkMatrixWidget::setValues(const QVector<double> & vector)
{
  Q_D(ctkMatrixWidget);
  Q_ASSERT(vector.size() == this->rowCount() * this->columnCount());
  // As we are potentially making a lot of changes, just fire a unique
  // signal at the end if at least one matrix value has been changed.
  bool blocked = this->blockSignals(true);
  bool modified = false;
  for (int row=0; row < this->rowCount(); ++row)
    {
    for (int column=0; column < this->columnCount(); ++column)
      {
      double newValue = vector.at(row * this->columnCount() + column);
      newValue = qBound(d->Minimum, newValue, d->Maximum);
      if (newValue != this->value(row, column))
        {
        this->setValue(row, column, newValue);
        modified = true;
        }
      }
    }
  this->blockSignals(blocked);
  if (modified)
    {
    this->emit matrixChanged();
    }
}

// --------------------------------------------------------------------------
QTableWidgetItem* ctkMatrixWidget::widgetItem(int i, int j)
{
  Q_D(ctkMatrixWidget);
  QTableWidgetItem* item = d->Table->item(i, j);
  return item;
}
