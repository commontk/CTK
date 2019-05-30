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

#ifndef __ctkMatrixWidget_h
#define __ctkMatrixWidget_h

/// Qt includes
#include <QVector>
#include <QWidget>

/// CTK includes
#include "ctkDoubleSpinBox.h"
#include "ctkPimpl.h"
#include "ctkWidgetsExport.h"

class ctkMatrixWidgetPrivate;
class QTableWidgetItem;

/// \ingroup Widgets
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
  /// This property controls how many decimals are used to display and edit the
  /// matrix values.
  /// \sa decimals(), setDecimals(), decimalsChanged(), decimalsOption
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals NOTIFY decimalsChanged)
  /// This property provides more controls over the decimals.
  /// \sa ctkDoubleSpinBox::DecimalsOptions, decimals
  Q_PROPERTY(ctkDoubleSpinBox::DecimalsOptions decimalsOption READ decimalsOption WRITE setDecimalsOption)
  Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep)
  Q_PROPERTY(QVector<double> values READ values WRITE setValues)

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
  Q_INVOKABLE double value(int i, int j)const;
  Q_INVOKABLE void setValue(int i, int j, double value);

  ///
  /// Provides low-level access to widget item of each matrix element.
  /// This may be used for customizing display or behavior of specific
  /// matrix elements.
  Q_INVOKABLE QTableWidgetItem* widgetItem(int i, int j);

  ///
  /// Utility function to set/get all the values of the matrix at once.
  /// Only one signal matrixChanged() is fired at the end.
  QVector<double> values()const;
  void setValues(const QVector<double> & vector);

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

  /// Return the decimalsOption property value
  /// \sa decimalsOption
  ctkDoubleSpinBox::DecimalsOptions decimalsOption()const;
  /// Set the decimalsOption property value.
  /// \sa decimalsOption
  void setDecimalsOption(ctkDoubleSpinBox::DecimalsOptions option);

  ///
  /// Reimplemented from QAbstractScrollArea
  virtual QSize minimumSizeHint () const;
  virtual QSize sizeHint () const;

public Q_SLOTS:

  ///
  /// Reset the matrix to identity
  void identity();

  ///
  /// Set how many decimals will be used for displaying and interpreting
  /// doubles by the spinbox used to adjust the value of a matrix element.
  void setDecimals(int decimals);

Q_SIGNALS:
  void matrixChanged();
  /// This signal is fired when the number of decimals is changed.
  /// This can be useful when synchronizing decimals between widgets.
  /// \sa decimals
  void decimalsChanged(int);

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
