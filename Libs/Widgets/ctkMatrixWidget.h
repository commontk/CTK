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

#ifndef __ctkMatrixWidget_h
#define __ctkMatrixWidget_h

/// Qt includes
#include <QTableWidget>

/// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"

class ctkMatrixWidgetPrivate;

///
/// ctkMatrixWidget is the base class of matrix widgets.
/// \todo Add a property to handle wether the user can edit values
/// \todo Wrap model signals to emit signals when the matrix is changed.
/// Right now you can connect to the signal:
/// matrixWidget->model()->dataChanged(...)
class CTK_WIDGETS_EXPORT ctkMatrixWidget : public QTableWidget
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef QTableWidget Superclass;

  /// Constructor, builds a 4x4 matrix
  explicit ctkMatrixWidget(QWidget* parent = 0);
  /// Constructor, builds a custom rowsXcolumns matrix
  explicit ctkMatrixWidget(int rows, int columns, QWidget* parent = 0);
  virtual ~ctkMatrixWidget(){}

  ///
  /// Set / Get values of the matrix
  /// \li i is the row index, \li j is the column index
  /// \warning there is no check that the indexes are inside their
  /// valid range
  double value(int i, int j)const;
  void setValue(int i, int j, double value);
  void setVector(const QVector<double> & vector);

  ///
  /// Reimplemented from QAbstractScrollArea
  virtual QSize minimumSizeHint () const;
  virtual QSize sizeHint () const;

public slots:
  ///
  /// Reset the matrix to identity
  void reset();

protected:
  ///
  /// Reimplemented from QTableView
  /// Share the width/height evenly between columns/rows.
  virtual void updateGeometries();

private:
  CTK_DECLARE_PRIVATE(ctkMatrixWidget);
};

#endif
