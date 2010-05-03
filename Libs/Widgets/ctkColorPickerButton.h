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

#ifndef __ctkColorPickerButton_h
#define __ctkColorPickerButton_h

// Qt includes
#include <QPushButton>
#include <QColor>

// CTK includes
#include "CTKWidgetsExport.h"

class CTK_WIDGETS_EXPORT ctkColorPickerButton : public QPushButton
{
  Q_OBJECT
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)
public:
  explicit ctkColorPickerButton(QWidget* parent = 0);
  explicit ctkColorPickerButton(const QString& text, QWidget* parent = 0 );
  explicit ctkColorPickerButton(const QColor& color, const QString & text, QWidget* parent = 0 );
  virtual ~ctkColorPickerButton(){}
  
  QColor color()const;

public slots:
  void setColor(const QColor& color);
  void changeColor(bool change = true);

signals:
  void colorChanged(QColor);

protected:
  QColor Color;
};

#endif
