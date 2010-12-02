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
#include "ctkWidgetsExport.h"

///
/// ctkCoordinatesWidget is a simple container of dimension coordinates.
/// For each coordinate a double spinbox is associated, everytime a value is
/// modified, the signal valueChanged is fired.
/// TODO: use pimpl
class CTK_WIDGETS_EXPORT ctkCoordinatesWidget : public QWidget
{
  Q_OBJECT 
  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double singleStep  READ singleStep WRITE setSingleStep STORED false)
  Q_PROPERTY(int dimension READ dimension WRITE setDimension)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)

  Q_PROPERTY(QString coordinates READ coordinatesAsString WRITE setCoordinatesAsString)

public:
  explicit ctkCoordinatesWidget(QWidget* parent = 0);
  virtual ~ctkCoordinatesWidget();

  /// 
  /// Set/Get the dimension of the point
  /// The default dimension is 3
  void setDimension(int dim);
  int dimension() const;
  
  /// 
  /// Set/Get the number of decimals of each coordinate QDoubleSpinBoxes 
  /// The default single step is 3
  void setDecimals(int decimals);
  int decimals() const;
  

  /// 
  /// Set/Get the single step of each coordinate QDoubleSpinBoxes 
  /// The default single step is 1.
  void setSingleStep(double step);
  double singleStep() const;

  /// 
  /// Set/Get the minimum value of each coordinate QDoubleSpinBoxes 
  /// The default minimum is -100000.
  void setMinimum(double minimum);
  double minimum() const;

  /// 
  /// Set/Get the maximum value of each coordinate QDoubleSpinBoxes 
  /// The default maximum is 100000.
  void setMaximum(double minimum);
  double maximum() const;

  /// 
  /// Set/Get the coordinates. Use commas between numbers
  /// i.e. "0,0.0,0." 
  void setCoordinatesAsString(QString pos);
  QString coordinatesAsString()const;

  /// 
  /// Set/Get the coordinates
  /// The default values are 0.
  void setCoordinates(double* pos);
  double const * coordinates()const;

signals:
  ///
  /// valueChanged is fired anytime a coordinate is modified, the returned
  /// value is the point coordinates
  /// TODO: Don't fire the signal if the new values are not changed 
  void coordinatesChanged(double* pos);

protected slots:
  void updateCoordinate(double);
  void updateCoordinates();

protected:
  void addSpinBox();

  int     Decimals;
  double  SingleStep;
  double  Minimum;
  double  Maximum;
  int     Dimension;
  double* Coordinates;
};

#endif
