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

#ifndef __ctkMatrixWidget_h
#define __ctkMatrixWidget_h

/// Qt includes
#include <QWidget>

/// CTK includes
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkMatrixWidgetPrivate;

///
/// ctkMatrixWidget is the base class of matrix widgets.
class CTK_WIDGETS_EXPORT ctkMatrixWidget: public QWidget
{
  Q_OBJECT
  Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount)
  Q_PROPERTY(int rowCount READ rowCount WRITE setRowCount)
  Q_PROPERTY(bool editable READ isEditable WRITE setEditable)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor, builds a 4x4 identity matrix
  explicit ctkMatrixWidget(QWidget* parent = 0);
  /// Constructor, builds a custom rowsXcolumns matrix
  explicit ctkMatrixWidget(int rows, int columns, QWidget* parent = 0);
  virtual ~ctkMatrixWidget();

  /// Set the number of columns of the matrix
  /// \sa rowCount, setRowCount
  int columnCount()const;
  virtual void setColumnCount(int newColumnCount);

  /// Set the number of rows of the matrix
  /// \sa columnCount, setColumnCount
  int rowCount()const;
  virtual void setRowCount(int newRowCount);

  ///
  /// Set / Get values of the matrix
  /// \li i is the row index, \li j is the column index
  /// \warning There is no check that the indexes are inside their
  /// valid range
  /// \warning The value of a matrix element will not be changed on an attempt to set it to a value
  /// that is less than the minimum or greater than the maximum.
  double value(int i, int j)const;
  void setValue(int i, int j, double value);
  void setVector(const QVector<double> & vector);

  ///
  /// This property determines whether the user can edit values by
  /// double clicking on the items. True by default
  bool isEditable()const;
  void setEditable(bool newEditable);

  ///
  /// This property holds the minimum value of matrix elements.
  ///
  /// Any matrix elements whose values are less than the new minimum value will be reset to equal
  /// the new minimum value.
  double minimum()const;
  void setMinimum(double newMinimum);

  ///
  /// This property holds the maximum value of matrix elements.
  ///
  /// Any matrix elements whose values are greater than the new maximum value will be reset to equal
  /// the new maximum value.
  double maximum()const;
  void setMaximum(double newMaximum);

  /// Description
  /// Utility function that sets the min/max at once.
  void setRange(double newMinimum, double newMaximum);

  ///
  /// This property holds the step value of the spinbox.
  ///
  /// When the user uses the arrows to change the value of the spinbox used to adjust the value of
  /// a matrix element, the value will be incremented/decremented by the amount of the singleStep.
  double singleStep()const;
  void setSingleStep(double step);

  ///
  /// This property holds the precision of the spinbox, in decimals.
  ///
  /// Dictates how many decimals will be used for displaying and interpreting doubles by the spinbox
  /// used to adjust the value of a matrix element.
  int decimals()const;
  void setDecimals(int decimals);

  ///
  /// Reimplemented from QAbstractScrollArea
  virtual QSize minimumSizeHint () const;
  virtual QSize sizeHint () const;

public slots:

  ///
  /// Reset the matrix to identity
  void identity();

signals:
  void matrixChanged();

protected:
  virtual void resizeEvent(QResizeEvent* event);

  ///
  /// protected constructor to derive private implementations
  ctkMatrixWidget(ctkMatrixWidgetPrivate& pvt, QWidget* parent=0);
private:
  QScopedPointer<ctkMatrixWidgetPrivate> d_ptr;
  Q_DECLARE_PRIVATE(ctkMatrixWidget);
  Q_DISABLE_COPY(ctkMatrixWidget);
};

#endif
