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

#ifndef __ctkAxesWidget_h
#define __ctkAxesWidget_h

// Qt includes
#include <QWidget>
#include <QStyle>
#include <QSize>

// CTK includes
#include <ctkPimpl.h>
#include "CTKWidgetsExport.h"

class ctkAxesWidgetPrivate;

class CTK_WIDGETS_EXPORT ctkAxesWidget : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(Axis currentAxis READ currentAxis WRITE setCurrentAxis NOTIFY currentAxisChanged)

public : 

  enum Axis
    {
    None=0,
    Right,
    Left,
    Superior,
    Inferior,
    Anterior,
    Posterior,
    };
  Q_ENUMS(Axis)
  
  ctkAxesWidget(QWidget *parent = 0);
  virtual ~ctkAxesWidget();

  ///
  /// Current selected axis. None by default. 
  Axis currentAxis() const;

signals:
  void currentAxisChanged(Axis axis);

public slots :
  void setCurrentAxis(Axis axis);

protected slots: 
  void paintEvent(QPaintEvent *);
  void mousePressEvent(QMouseEvent *mouseEvent); 

protected:
  QScopedPointer<ctkAxesWidgetPrivate> d_ptr;
private :
  Q_DECLARE_PRIVATE(ctkAxesWidget);
  Q_DISABLE_COPY(ctkAxesWidget);
};


#endif