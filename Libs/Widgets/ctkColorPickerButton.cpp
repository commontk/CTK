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

// Qt includes
#include <QColorDialog>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QStyle>

// CTK includes
#include "ctkColorPickerButton.h"

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(QWidget* _parent)
  :QPushButton(_parent)
{
  connect(this, SIGNAL(toggled(bool)), this, SLOT(changeColor(bool)));
  this->setColor(Qt::black);

  this->setCheckable(true);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(const QString& _text, QWidget* _parent)
  :QPushButton(_text, _parent)
{
  connect(this, SIGNAL(clicked), this, SLOT(changeColor));
  this->setColor(Qt::black);

  // Customize
  this->setCheckable(true);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(const QColor& _color,
                                             const QString& _text,
                                             QWidget* _parent)
  :QPushButton(_text, _parent)
{
  connect(this, SIGNAL(clicked), this, SLOT(changeColor));
  this->setColor(_color);

  // Customize
  this->setCheckable(true);
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::changeColor(bool change)
{
  if (change)
    {
    this->setColor(QColorDialog::getColor(this->Color));

    this->setChecked(false);
    }
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::setColor(const QColor& _color)
{
  if (_color == this->Color)
    {
    return;
    }

  int _iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
  QPixmap pix(_iconSize, _iconSize);
  pix.fill(palette().button().color());
  QPainter p(&pix);
  p.setPen(QPen(Qt::gray));
  p.setBrush(_color);
  p.drawRect(2, 2, pix.width() - 5, pix.height() - 5);

  this->setIcon(QIcon(pix));
  this->setText(_color.name());
  
  this->Color = _color;
  emit colorChanged(this->Color);
}

//-----------------------------------------------------------------------------
QColor ctkColorPickerButton::color()const
{
  return this->Color;
}
