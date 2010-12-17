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

// Qt includes
#include <QColorDialog>
#include <QDebug>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>

// CTK includes
#include "ctkColorDialog.h"
#include "ctkColorPickerButton.h"

class ctkColorPickerButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkColorPickerButton);
protected:
  ctkColorPickerButton* const q_ptr;
public:
  ctkColorPickerButtonPrivate(ctkColorPickerButton& object);
  void init();
  void computeIcon();

  QIcon  Icon;
  QColor Color;
  bool   DisplayColorName;
  ctkColorPickerButton::ColorDialogOptions DialogOptions;
};

//-----------------------------------------------------------------------------
ctkColorPickerButtonPrivate::ctkColorPickerButtonPrivate(ctkColorPickerButton& object)
  : q_ptr(&object)
{
  this->Color = Qt::black;
  this->DisplayColorName = true;
  this->DialogOptions = 0;
}

//-----------------------------------------------------------------------------
void ctkColorPickerButtonPrivate::init()
{
  Q_Q(ctkColorPickerButton);
  q->setCheckable(true);
  QObject::connect(q, SIGNAL(toggled(bool)),
                   q, SLOT(onToggled(bool)));
  this->computeIcon();
}

//-----------------------------------------------------------------------------
void ctkColorPickerButtonPrivate::computeIcon()
{
  Q_Q(ctkColorPickerButton);
  int _iconSize = q->style()->pixelMetric(QStyle::PM_SmallIconSize);
  QPixmap pix(_iconSize, _iconSize);
  pix.fill(q->palette().button().color());
  QPainter p(&pix);
  p.setPen(QPen(Qt::gray));
  p.setBrush(this->Color);
  p.drawRect(2, 2, pix.width() - 5, pix.height() - 5);

  this->Icon = QIcon(pix);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(QWidget* _parent)
  : QPushButton(_parent)
  , d_ptr(new ctkColorPickerButtonPrivate(*this))
{
  Q_D(ctkColorPickerButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(const QString& _text, QWidget* _parent)
  : QPushButton(_text, _parent)
  , d_ptr(new ctkColorPickerButtonPrivate(*this))
{
  Q_D(ctkColorPickerButton);
  d->init();
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ctkColorPickerButton(const QColor& _color,
                                           const QString& _text,
                                           QWidget* _parent)
  : QPushButton(_text, _parent)
  , d_ptr(new ctkColorPickerButtonPrivate(*this))
{
  Q_D(ctkColorPickerButton);
  d->init();
  this->setColor(_color);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::~ctkColorPickerButton()
{
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::changeColor()
{
  Q_D(ctkColorPickerButton);
  QColor res;
    QColorDialog::ColorDialogOptions options;
    options |= QColorDialog::ColorDialogOption(
      static_cast<int>(d->DialogOptions & ShowAlphaChannel));
    options |= QColorDialog::ColorDialogOption(
      static_cast<int>(d->DialogOptions & NoButtons));
    options |= QColorDialog::ColorDialogOption(
      static_cast<int>(d->DialogOptions & DontUseNativeDialog));
  if (d->DialogOptions & UseCTKColorDialog)
    {
    res = ctkColorDialog::getColor(d->Color, this, QString(""),options);
    }
  else
    {
    res = QColorDialog::getColor(d->Color, this, QString(""), options);
    }
  if (res.isValid())
    {
    this->setColor(res);
    }
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
void ctkColorPickerButton::setDisplayColorName(bool displayColorName)
{
  Q_D(ctkColorPickerButton);
  d->DisplayColorName = displayColorName;
  this->update();
}

//-----------------------------------------------------------------------------
bool ctkColorPickerButton::displayColorName()const
{
  Q_D(const ctkColorPickerButton);
  return d->DisplayColorName;
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::setDialogOptions(const ColorDialogOptions& options)
{
  Q_D(ctkColorPickerButton);
  d->DialogOptions = options;
}

//-----------------------------------------------------------------------------
const ctkColorPickerButton::ColorDialogOptions& ctkColorPickerButton::dialogOptions()const
{
  Q_D(const ctkColorPickerButton);
  return d->DialogOptions;
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::setColor(const QColor& newColor)
{
  Q_D(ctkColorPickerButton);
  if (newColor == d->Color)
    {
    return;
    }

  d->Color = newColor;
  d->computeIcon();

  this->update();
  emit colorChanged(d->Color);
}

//-----------------------------------------------------------------------------
QColor ctkColorPickerButton::color()const
{
  Q_D(const ctkColorPickerButton);
  return d->Color;
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::paintEvent(QPaintEvent *)
{
  Q_D(ctkColorPickerButton);
  QStylePainter p(this);
  QStyleOptionButton option;
  initStyleOption(&option);
  if (d->DisplayColorName)
    {
    option.text = d->Color.name();
    }
  option.icon = d->Icon;
  p.drawControl(QStyle::CE_PushButton, option);
}
