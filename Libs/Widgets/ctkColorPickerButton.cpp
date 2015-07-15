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

// Qt includes
#include <QApplication>
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
  QString text()const;

  QIcon  Icon;
  QColor Color;
  QString ColorName;
  bool   DisplayColorName;
  ctkColorPickerButton::ColorDialogOptions DialogOptions;
  mutable QSize CachedSizeHint;
};

//-----------------------------------------------------------------------------
ctkColorPickerButtonPrivate::ctkColorPickerButtonPrivate(ctkColorPickerButton& object)
  : q_ptr(&object)
{
  this->Color = Qt::black;
  this->ColorName = QString();
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
  pix.fill(this->Color.isValid() ?
    q->palette().button().color() : Qt::transparent);
  QPainter p(&pix);
  p.setPen(QPen(Qt::gray));
  p.setBrush(this->Color.isValid() ?
    this->Color : QBrush(Qt::NoBrush));
  p.drawRect(2, 2, pix.width() - 5, pix.height() - 5);

  this->Icon = QIcon(pix);
}

//-----------------------------------------------------------------------------
QString ctkColorPickerButtonPrivate::text()const
{
  Q_Q(const ctkColorPickerButton);
  if (!this->DisplayColorName)
    {
    return q->text();
    }
  if (this->ColorName.isEmpty())
    {
    return this->Color.name();
    }
  else
    {
    return this->ColorName;
    }
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
  QColor newColor;
  QString newColorName;
  QColorDialog::ColorDialogOptions options;
  options |= QColorDialog::ColorDialogOption(
    static_cast<int>(d->DialogOptions & ShowAlphaChannel));
  options |= QColorDialog::ColorDialogOption(
    static_cast<int>(d->DialogOptions & NoButtons));
  options |= QColorDialog::ColorDialogOption(
    static_cast<int>(d->DialogOptions & DontUseNativeDialog));
  if (d->DialogOptions & UseCTKColorDialog)
    {
    newColor = ctkColorDialog::getColor(d->Color, this, QString(""),options);
    newColorName = ctkColorDialog::getColorName();
    }
  else
    {
    newColor = QColorDialog::getColor(d->Color, this, QString(""), options);
    }
  if (newColor.isValid())
    {
    this->setColor(newColor);
    this->setColorName(newColorName);
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
  d->CachedSizeHint = QSize();
  this->update();
  this->updateGeometry();
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
void ctkColorPickerButton::setColorName(const QString& newColorName)
{
  Q_D(ctkColorPickerButton);
  if (newColorName == d->ColorName)
    {
    return;
    }

  d->ColorName = newColorName;
  d->CachedSizeHint = QSize();
  this->update();
  this->updateGeometry();
  emit colorNameChanged(d->ColorName);
}

//-----------------------------------------------------------------------------
QString ctkColorPickerButton::colorName()const
{
  Q_D(const ctkColorPickerButton);
  return d->ColorName;
}

//-----------------------------------------------------------------------------
void ctkColorPickerButton::paintEvent(QPaintEvent *)
{
  Q_D(ctkColorPickerButton);
  QStylePainter p(this);
  QStyleOptionButton option;
  this->initStyleOption(&option);
  option.text = d->text();
  option.icon = d->Icon;
  p.drawControl(QStyle::CE_PushButton, option);
}

//-----------------------------------------------------------------------------
QSize ctkColorPickerButton::sizeHint()const
{
  Q_D(const ctkColorPickerButton);
  if (!d->DisplayColorName && !this->text().isEmpty())
    {
    return this->QPushButton::sizeHint();
    }
  if (d->CachedSizeHint.isValid())
    {
    return d->CachedSizeHint;
    }

  // If no text, the sizehint is a QToolButton sizeHint
  QStyleOptionButton pushButtonOpt;
  this->initStyleOption(&pushButtonOpt);
  pushButtonOpt.text = d->text();
  int iconSize = this->style()->pixelMetric(QStyle::PM_SmallIconSize);
  if (pushButtonOpt.text == QString())
    {
    QStyleOptionToolButton opt;
    (&opt)->QStyleOption::operator=(pushButtonOpt);
    opt.arrowType = Qt::NoArrow;
    opt.icon = d->Icon;
    opt.iconSize = QSize(iconSize, iconSize);
    opt.rect.setSize(opt.iconSize); // PM_MenuButtonIndicator depends on the height
    d->CachedSizeHint = this->style()->sizeFromContents(
      QStyle::CT_ToolButton, &opt, opt.iconSize, this).
      expandedTo(QApplication::globalStrut());
    }
  else
    {
    pushButtonOpt.icon = d->Icon;
    pushButtonOpt.iconSize = QSize(iconSize, iconSize);
    pushButtonOpt.rect.setSize(pushButtonOpt.iconSize); // PM_MenuButtonIndicator depends on the height
    d->CachedSizeHint = (style()->sizeFromContents(
                           QStyle::CT_PushButton, &pushButtonOpt, pushButtonOpt.iconSize, this).
                         expandedTo(QApplication::globalStrut()));
    }
  return d->CachedSizeHint;
}
