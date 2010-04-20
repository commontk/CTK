/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkMatrixWidget_h
#define __ctkMatrixWidget_h

/// QT includes
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
