/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
