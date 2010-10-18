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

#ifndef __ctkCoordinatesWidget_h
#define __ctkCoordinatesWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "CTKWidgetsExport.h"

///
/// ctkCoordinatesWidget is a simple container of dimension coordinates.
/// For each coordinate a double spinbox is associated, everytime a value is
/// modified, the signal valueChanged is fired.
class CTK_WIDGETS_EXPORT ctkCoordinatesWidget : public QWidget
{
  Q_OBJECT 
  Q_PROPERTY(int Dimension READ dimension WRITE setDimension)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)
  Q_PROPERTY(double singleStep  READ singleStep WRITE setSingleStep STORED false)
  Q_PROPERTY(QString coordinates READ coordinatesAsString WRITE setCoordinatesAsString)
    
public:
  explicit ctkCoordinatesWidget(QWidget* parent = 0);
  virtual ~ctkCoordinatesWidget();

  /// 
  /// Set/Get the dimension of the point (3 by default)
  void setDimension(int dim);
  int dimension() const;

  /// 
  /// Set/Get the single step of each coordinate QDoubleSpinBoxes 
  void setSingleStep(double step);
  double singleStep() const;

  /// 
  /// Set/Get the minimum value of each coordinate QDoubleSpinBoxes 
  void setMinimum(double minimum);
  double minimum() const;

  /// 
  /// Set/Get the maximum value of each coordinate QDoubleSpinBoxes 
  void setMaximum(double minimum);
  double maximum() const;

  /// 
  /// Set/Get the coordinates. Use commas between numbers
  void setCoordinatesAsString(QString pos);
  QString coordinatesAsString()const;

  /// 
  /// Set/Get the coordinates
  void setCoordinates(double* pos);
  double* coordinates()const;

signals:
  ///
  /// valueChanged is fired anytime a coordinate is modified, the returned
  /// value is the point coordinates
  void valueChanged(double* pos);

protected slots:
  void coordinateChanged(double);
  void coordinatesChanged();

protected:
  void addSpinBox();

  double  Minimum;
  double  Maximum;
  double  SingleStep;
  int     Dimension;
  double* Coordinates;
};

#endif
