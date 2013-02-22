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

#ifndef __ctkCoordinatesWidget_h
#define __ctkCoordinatesWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
///
/// ctkCoordinatesWidget is a simple container of dimension coordinates.
/// For each coordinate a double spinbox is associated, everytime a value is
/// modified, the signal valueChanged is fired.
/// TODO: use pimpl
class CTK_WIDGETS_EXPORT ctkCoordinatesWidget : public QWidget
{
  Q_OBJECT

  Q_PROPERTY(int dimension READ dimension WRITE setDimension)
  /// This property controls whether the coordinates must be normalized.
  /// If true, the norm of the coordinates is enforced to be 1.
  /// False by default.
  Q_PROPERTY(bool normalized READ isNormalized WRITE setNormalized)

  Q_PROPERTY(int decimals READ decimals WRITE setDecimals)
  Q_PROPERTY(double singleStep  READ singleStep WRITE setSingleStep STORED false)
  Q_PROPERTY(double minimum READ minimum WRITE setMinimum)
  Q_PROPERTY(double maximum READ maximum WRITE setMaximum)

  Q_PROPERTY(QString coordinates READ coordinatesAsString WRITE setCoordinatesAsString)

public:
  explicit ctkCoordinatesWidget(QWidget* parent = 0);
  virtual ~ctkCoordinatesWidget();

  /// Set/Get the dimension of the point
  /// The default dimension is 3
  void setDimension(int dim);
  int dimension() const;

  /// Set/Get the number of decimals of each coordinate QDoubleSpinBoxes
  /// The default number of decimals is 3.
  void setDecimals(int decimals);
  int decimals() const;

  /// Set/Get the single step of each coordinate QDoubleSpinBoxes
  /// The default single step is 1.
  void setSingleStep(double step);
  double singleStep() const;

  /// Set/Get the minimum value of each coordinate QDoubleSpinBoxes
  /// The default minimum is -100000.
  void setMinimum(double minimum);
  double minimum() const;

  /// Set/Get the maximum value of each coordinate QDoubleSpinBoxes
  /// The default maximum is 100000.
  void setMaximum(double minimum);
  double maximum() const;

  /// Change the normalized property. If \a normalize is true, it normalizes
  /// the current coordinates, the range of possible values is reset to [-1, 1].
  /// \sa isNormalized
  void setNormalized(bool normalize);
  bool isNormalized()const;

  /// Return the norm of the coordinates.
  double norm()const;
  /// Return the squared norm of the coordinates.
  double squaredNorm()const;

  /// Set/Get the coordinates. Use commas to separate elements, spaces are
  /// allowed: e.g. "0,0.0, 0."
  void setCoordinatesAsString(QString pos);
  QString coordinatesAsString()const;

  /// Set/Get the coordinates
  /// The default values are 0.
  void setCoordinates(double* pos);
  double const * coordinates()const;

  /// Convenient function that sets up to 4 elements of the coordinates.
  void setCoordinates(double x, double y = 0., double z = 0., double w = 0.);

  /// Set/Get the keyboardTracking property of the coordinates widget
  /// If keyboardTracking is enabled (the default), the coordinatesWidget emits
  /// the coordinatesChanged() signal while the new value is being entered from
  /// the keyboard.
  /// E.g. when the user enters the value 600 by typing 6, 0, and 0,
  /// the spinbox emits 3 signals with the values 6, 60, and 600 respectively.
  /// \sa coordinatesChanged(double* pos)
  void setKeyboardTracking(bool enable);
  bool keyboardTracking()const;

public Q_SLOTS:
  void normalize();

Q_SIGNALS:
  ///
  /// coordinatesChanged is fired anytime a coordinate is modified, the returned
  /// value is the point coordinates.
  /// This signal will be fired while the user is editing a coordinate or
  /// once he/she is done editing based on keyboardTracking flag.
  /// TODO: Don't fire the signal if the new values are not changed
  /// \sa keyboardTracking(), setKeyboardTracking(bool enable)
  void coordinatesChanged(double* pos);

  /// coordinatesEditingFinished is fired anytime a user modifies a coordinate
  /// and is done editing. This happens when the corresponding spinBox loses
  /// focus or enter is pressed. The returned value is the point coordinates.
  /// \sa coordinatesChanged(double* pos)
  void coordinatesEditingFinished(double* pos);

protected Q_SLOTS:
  ///
  /// updateCoordinate is invoked whenever any spinBox in the
  /// ctkCoordinatesWidget emits an editingFinished() signal. This slot is
  /// responsible for emitting the coordinatesEditingFinished() signal.
  /// \sa coordinatesEditingFinished(double* pos)
  void updateCoordinate();
  
  /// coordinateValueChanged() is invoked whenever any spinBox in the
  /// ctkCoordinatesWidget emits a valueChanged() signal. This slot is
  /// responsible for emitting the coordinatesChanged() signal.
  /// \sa coordinatesChanged(double* pos)
  void coordinateValueChanged(double);

protected:
  void addSpinBox();

  /// Update the coordinate according to the new value
  void updateCoordinate(double);

  /// Update all coordinates based on any value change
  void updateCoordinates();

  /// Normalize coordinates vector and return the previous norm.
  static double normalize(double* coordinates, int dimension);

  /// Compute the norm of a coordinates \a dimension vector
  static double norm(double* coordinates, int dimension);
  static double squaredNorm(double* coordinates, int dimension);

  int     Decimals;
  double  SingleStep;
  double  Minimum;
  double  Maximum;
  bool    Normalized;
  int     Dimension;
  double* Coordinates;
  bool    KeyboardTracking;
  QList<int> LastUserEditedCoordinates;
};

#endif
