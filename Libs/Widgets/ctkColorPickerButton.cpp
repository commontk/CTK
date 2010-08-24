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
  connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
  this->setColor(Qt::black);

  this->setCheckable(true);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(const QString& _text, QWidget* _parent)
  :QPushButton(_text, _parent)
{
  connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
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
  connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
  this->setColor(_color);

  // Customize
  this->setCheckable(true);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::~ctkColorPickerButton()
{
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::changeColor()
{
  this->setColor(QColorDialog::getColor(this->Color));
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::onToggled(bool change)
{
  if (change)
    {
    this->changeColor();
    this->setChecked(false);
    }
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::setColor(const QColor& newColor)
{
  if (newColor == this->Color)
    {
    return;
    }

  int _iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
  QPixmap pix(_iconSize, _iconSize);
  pix.fill(palette().button().color());
  QPainter p(&pix);
  p.setPen(QPen(Qt::gray));
  p.setBrush(newColor);
  p.drawRect(2, 2, pix.width() - 5, pix.height() - 5);

  this->setIcon(QIcon(pix));
  this->setText(newColor.name());
  
  this->Color = newColor;
  emit colorChanged(this->Color);
}

//-----------------------------------------------------------------------------
QColor ctkColorPickerButton::color()const
{
  return this->Color;
}
