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
#include <QApplication>
#include <QDebug>
#include <QFontDialog>

// CTK includes
#include "ctkFontButton.h"

//-----------------------------------------------------------------------------
class ctkFontButtonPrivate
{
  Q_DECLARE_PUBLIC(ctkFontButton);

protected:
  ctkFontButton* const q_ptr;

public:
  ctkFontButtonPrivate(ctkFontButton& object);
  void init();

  QFont Font;
};

//-----------------------------------------------------------------------------
ctkFontButtonPrivate::ctkFontButtonPrivate(ctkFontButton& object)
  :q_ptr(&object)
{
  this->Font = qApp->font();
}

//-----------------------------------------------------------------------------
void ctkFontButtonPrivate::init()
{
  Q_Q(ctkFontButton);
  q->setFont(this->Font);
  q->setText(this->Font.family());
  
  QObject::connect(q, SIGNAL(clicked()), q, SLOT(browseFont()));
}

//-----------------------------------------------------------------------------
ctkFontButton::ctkFontButton(QWidget * parentWidget)
  : QPushButton(parentWidget)
  , d_ptr(new ctkFontButtonPrivate(*this))
{
  Q_D(ctkFontButton);
  d->init();
}
    
//-----------------------------------------------------------------------------
ctkFontButton::ctkFontButton(const QFont& font, 
                             QWidget * parentWidget)
  : QPushButton(parentWidget)
  , d_ptr(new ctkFontButtonPrivate(*this))
{
  Q_D(ctkFontButton);
  d->init();
  this->setFont(font);
}

//-----------------------------------------------------------------------------
ctkFontButton::~ctkFontButton()
{
}

//-----------------------------------------------------------------------------
void ctkFontButton::setCurrentFont(const QFont& newFont)
{
  Q_D(ctkFontButton);

  if (d->Font == newFont)
    {
    return;
    }

  d->Font = newFont;

  this->setFont(newFont);
  this->setText(newFont.family());

  emit currentFontChanged(newFont);
}

//-----------------------------------------------------------------------------
QFont ctkFontButton::currentFont()const
{
  Q_D(const ctkFontButton);
  return d->Font;
}

//-----------------------------------------------------------------------------
void ctkFontButton::browseFont()
{
  Q_D(ctkFontButton);
  bool ok = false;
  QFont newFont = QFontDialog::getFont(&ok, d->Font, this);
  if (ok)
    {
    this->setCurrentFont(newFont);
    }
}
