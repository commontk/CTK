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
// QT includes
#include <QApplication>
#include <QProxyStyle>
#include <QStyleOption>

// CTK includes
#include "ctkCheckBox.h"

#include <iostream>

// ----------------------------------------------------------------------------
class ctkCheckBoxStyle : public QProxyStyle
{
  public:
  ctkCheckBoxStyle(QStyle *parentStyle)
    : QProxyStyle(parentStyle)
    {}

  void setCheckIcon(const QIcon& newIcon)
  {
    this->checkBoxIcon = newIcon;
  }
  QIcon checkIcon() const
  {
    return this->checkBoxIcon;
  }

  virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption * opt, QPainter * p, const QWidget * widget = 0) const
  {
    if (pe == QStyle::PE_IndicatorCheckBox)
      {
      const ctkCheckBox* checkBox= qobject_cast<const ctkCheckBox*>(widget);
      if (checkBox && !checkBoxIcon.isNull())
        {
        QIcon::Mode mode =
            (checkBox->testAttribute(Qt::WA_Hover) && checkBox->underMouse()) ?
              QIcon::Active : QIcon::Normal;
        mode = checkBox->isDown() ? QIcon::Selected : mode;
        mode = checkBox->isEnabled() ? mode : QIcon::Disabled;
        this->drawItemPixmap(p, opt->rect, Qt::AlignHCenter,
                             checkBoxIcon.pixmap(
                               opt->rect.width(), opt->rect.height(),
                               mode,
                               checkBox->isChecked() ? QIcon::Off : QIcon::On));
        return;
        }
      }
    this->QProxyStyle::drawPrimitive(pe, opt, p, widget);
  }
protected:
  QIcon   checkBoxIcon;
};

// ----------------------------------------------------------------------------
class ctkCheckBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkCheckBox);
protected:
  ctkCheckBox* const q_ptr;
public:
  ctkCheckBoxPrivate(ctkCheckBox& object);
  void init();

  ctkCheckBoxStyle* iconStyle;
};

// ----------------------------------------------------------------------------
ctkCheckBoxPrivate::ctkCheckBoxPrivate(ctkCheckBox &object)
  : q_ptr(&object)
{
  this->iconStyle = 0;
}

// ----------------------------------------------------------------------------
void ctkCheckBoxPrivate::init()
{
  Q_Q(ctkCheckBox);
  this->iconStyle = new ctkCheckBoxStyle(0);
  this->iconStyle->setParent(q);
  q->setStyle(this->iconStyle);
}

// ----------------------------------------------------------------------------
//  Methods ctkCheckBox

// ----------------------------------------------------------------------------
ctkCheckBox::ctkCheckBox(QWidget *_parent)
  :Superclass(_parent)
  , d_ptr(new ctkCheckBoxPrivate(*this))
{
  Q_D(ctkCheckBox);
  d->init();
}

// ----------------------------------------------------------------------------
ctkCheckBox::~ctkCheckBox()
{
}

// ----------------------------------------------------------------------------
void ctkCheckBox::setCheckIcon(const QIcon& newIcon)
{
  Q_D(ctkCheckBox);
  d->iconStyle->setCheckIcon(newIcon);
  this->update();
}

// ----------------------------------------------------------------------------
QIcon ctkCheckBox::checkIcon() const
{
  Q_D(const ctkCheckBox);
  return d->iconStyle->checkIcon();
}
