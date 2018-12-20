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

#ifndef __ctkCoordinatesWidget_p_h
#define __ctkCoordinatesWidget_p_h

// Qt includes
#include <QPointer>

// CTK includes
#include "ctkCoordinatesWidget.h"

class ctkCoordinatesWidgetPrivate: public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(ctkCoordinatesWidget);
protected:
  ctkCoordinatesWidget* const q_ptr;
public:
  ctkCoordinatesWidgetPrivate(ctkCoordinatesWidget& object);
  virtual ~ctkCoordinatesWidgetPrivate();

  void init();
public Q_SLOTS:
  void updateDecimals();
  void updateOtherDecimals(int);
  void setTemporaryDecimals(int);
  void onValueProxyAboutToBeModified();
  void onValueProxyModified();

public:
  void addSpinBox();

  /// Normalize coordinates vector and return the previous norm.
  static double normalize(double* coordinates, int dimension);

  /// Compute the norm of a coordinates \a dimension vector
  static double norm(double* coordinates, int dimension);
  static double squaredNorm(double* coordinates, int dimension);

  /// Return the ideal number of decimals based on the spinBox value or
  /// 16 if there is no "good" number of decimals.
  /// \sa ctk::significantDecimals()
  static int spinBoxSignificantDecimals(ctkDoubleSpinBox* spinBox);

  int     Decimals;
  ctkDoubleSpinBox::DecimalsOptions DecimalsOption;
  double  SingleStep;
  double  Minimum;
  double  Maximum;
  bool    Normalized;
  int     Dimension;
  ctkDoubleSpinBox::SizeHintPolicy SizeHintPolicy;
  bool    Frame;
  bool    ReadOnly;

  double* Coordinates;
  QList<int> LastUserEditedCoordinates;
  bool    ChangingDecimals;
  QPointer<ctkValueProxy> Proxy;

private:
  Q_DISABLE_COPY(ctkCoordinatesWidgetPrivate);

};

#endif
