/*=========================================================================

  Library:   CTK
 
  Copyright (c) Kitware Inc.

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

#ifndef __ctkMatrixWidget_h
#define __ctkMatrixWidget_h

/// Qt includes
#include <QTableWidget>

/// CTK includes
#include "ctkPimpl.h"
#include "CTKWidgetsExport.h"

class ctkMatrixWidgetPrivate;

class CTK_WIDGETS_EXPORT ctkMatrixWidget : public QTableWidget
{
  Q_OBJECT

public:
  /// Superclass typedef
  typedef QTableWidget Superclass;

  /// Constructors
  explicit ctkMatrixWidget(QWidget* parent = 0);
  virtual ~ctkMatrixWidget(){}

  /// 
  /// Set / Get values
  double value(int i, int j);
  void setValue(int i, int j, double value);
  void setVector(const QVector<double> & vector);

  /// 
  /// Overloaded - See QWidget
  virtual QSize minimumSizeHint () const;
  virtual QSize sizeHint () const;


public slots:

  /// 
  /// Reset to zero
  void reset();

protected slots:
  /// 
  /// Adjust columns/rows size according to width/height
  void adjustRowsColumnsSize(int width, int height);

protected:
  /// 
  virtual void resizeEvent(QResizeEvent * event);

private:
  CTK_DECLARE_PRIVATE(ctkMatrixWidget);
};

#endif
