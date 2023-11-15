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
#include <QDebug>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QRect>
#include <QStyleOption>

// CTK includes
#include "ctkSearchBox.h"

// --------------------------------------------------
class ctkSearchBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkSearchBox);
protected:
  ctkSearchBox* const q_ptr;
public:
  ctkSearchBoxPrivate(ctkSearchBox& object);
  void init();

  /// Position and size for the clear icon in the QLineEdit
  QRect clearRect()const;
  /// Position and size for the search icon in the QLineEdit
  QRect searchRect()const;

  QIcon clearIcon;
  QIcon searchIcon;
  bool showSearchIcon;
  bool alwaysShowClearIcon;
  bool hideClearIcon;
};

// --------------------------------------------------
ctkSearchBoxPrivate::ctkSearchBoxPrivate(ctkSearchBox &object)
  : q_ptr(&object)
{
  this->clearIcon = QIcon(":Icons/clear2.svg");
  this->searchIcon = QIcon(":Icons/search.svg");
  this->showSearchIcon = false;
  this->alwaysShowClearIcon = false;
  this->hideClearIcon = true;
}

// --------------------------------------------------
void ctkSearchBoxPrivate::init()
{
  Q_Q(ctkSearchBox);

  // Set a text by default on the QLineEdit
  q->setPlaceholderText(ctkSearchBox::tr("Search..."));

  QObject::connect(q, SIGNAL(textChanged(QString)),
                   q, SLOT(updateClearButtonState()));
}

// --------------------------------------------------
QRect ctkSearchBoxPrivate::clearRect()const
{
  Q_Q(const ctkSearchBox);
  QRect cRect = this->searchRect();
  cRect.moveLeft(q->width() - cRect.width() - cRect.left());
  return cRect;
}

// --------------------------------------------------
QRect ctkSearchBoxPrivate::searchRect()const
{
  Q_Q(const ctkSearchBox);
  QRect sRect = q->contentsRect();
  // If the QLineEdit has a frame, the icon must be shifted from
  // the frame line width
  if (q->hasFrame())
    {
    QStyleOptionFrame opt;
    q->initStyleOption(&opt);
    sRect.adjust(opt.lineWidth, opt.lineWidth, -opt.lineWidth, -opt.lineWidth);
    }
  // Hardcoded: shrink by 1 pixel because some styles have a focus frame inside
  // the line edit frame.
  sRect.adjust(1, 1, -1, -1);
  // Square size
  sRect.setWidth(sRect.height());
  return sRect;
}

// --------------------------------------------------
ctkSearchBox::ctkSearchBox(QWidget* _parent)
  : QLineEdit(_parent)
  , d_ptr(new ctkSearchBoxPrivate(*this))
{
  Q_D(ctkSearchBox);
  d->init();
}

// --------------------------------------------------
ctkSearchBox::~ctkSearchBox()
{
}

// --------------------------------------------------
void ctkSearchBox::setShowSearchIcon(bool show)
{
  Q_D(ctkSearchBox);
  d->showSearchIcon = show;
  this->update();
}

// --------------------------------------------------
bool ctkSearchBox::showSearchIcon()const
{
  Q_D(const ctkSearchBox);
  return d->showSearchIcon;
}

// --------------------------------------------------
void ctkSearchBox::setAlwaysShowClearIcon(bool show)
{
  Q_D(ctkSearchBox);
  d->alwaysShowClearIcon = show;
  if (show == true)
    {
    d->hideClearIcon = false;
    }
  this->update();
}

// --------------------------------------------------
bool ctkSearchBox::alwaysShowClearIcon()const
{
  Q_D(const ctkSearchBox);
  return d->alwaysShowClearIcon;
}

// --------------------------------------------------
void ctkSearchBox::setSearchIcon(const QIcon& icon)
{
  Q_D(ctkSearchBox);
  d->searchIcon = icon;
  this->update();
}

// --------------------------------------------------
QIcon ctkSearchBox::searchIcon()const
{
  Q_D(const ctkSearchBox);
  return d->searchIcon;
}

// --------------------------------------------------
void ctkSearchBox::setClearIcon(const QIcon& icon)
{
  Q_D(ctkSearchBox);
  d->clearIcon = icon;
  this->update();
}

// --------------------------------------------------
QIcon ctkSearchBox::clearIcon()const
{
  Q_D(const ctkSearchBox);
  return d->clearIcon;
}

// --------------------------------------------------
void ctkSearchBox::paintEvent(QPaintEvent * event)
{
  Q_D(ctkSearchBox);

  // Draw the line edit with text.
  // Text has already been shifted to the right (in resizeEvent()) to leave
  // space for the search icon.
  this->Superclass::paintEvent(event);

  QPainter p(this);

  QRect cRect = d->clearRect();
  QRect sRect = d->showSearchIcon ? d->searchRect() : QRect();

  // Draw clearIcon
  if (!d->hideClearIcon)
    {
    QPixmap closePixmap = d->clearIcon.pixmap(cRect.size(),this->isEnabled() ? QIcon::Normal : QIcon::Disabled);
    this->style()->drawItemPixmap(&p, cRect, Qt::AlignCenter, closePixmap);
    }

  // Draw searchIcon
  if (d->showSearchIcon)
    {
    QPixmap searchPixmap = d->searchIcon.pixmap(sRect.size(), this->isEnabled() ? QIcon::Normal : QIcon::Disabled);
    this->style()->drawItemPixmap(&p, sRect, Qt::AlignCenter, searchPixmap);
    }
}

// --------------------------------------------------
void ctkSearchBox::mousePressEvent(QMouseEvent *e)
{
  Q_D(ctkSearchBox);

  if(d->clearRect().contains(e->pos()))
    {
    this->clear();
    emit this->textEdited(this->text());
    return;
    }

  if(d->showSearchIcon && d->searchRect().contains(e->pos()))
    {
    this->selectAll();
    return;
    }
  
  this->Superclass::mousePressEvent(e);
}

// --------------------------------------------------
void ctkSearchBox::mouseMoveEvent(QMouseEvent *e)
{
  Q_D(ctkSearchBox);

  if(d->clearRect().contains(e->pos()) ||
     (d->showSearchIcon && d->searchRect().contains(e->pos())))
    {
    this->setCursor(Qt::PointingHandCursor);
    }
  else
    {
    this->setCursor(this->isReadOnly() ? Qt::ArrowCursor : Qt::IBeamCursor);
    }
  this->Superclass::mouseMoveEvent(e);
}

// --------------------------------------------------
void ctkSearchBox::resizeEvent(QResizeEvent * event)
{
  Q_D(ctkSearchBox);
  static int iconSpacing = 0; // hardcoded,
  QRect cRect = d->clearRect();
  QRect sRect = d->showSearchIcon ? d->searchRect() : QRect();
  // Set 2 margins each sides of the QLineEdit, according to the icons
  this->setTextMargins( sRect.right() + iconSpacing, 0,
                        event->size().width() - cRect.left() - iconSpacing,0);
}

// --------------------------------------------------
void ctkSearchBox::updateClearButtonState()
{
  Q_D(ctkSearchBox);
  if (!d->alwaysShowClearIcon)
    {
    d->hideClearIcon = this->text().isEmpty() ? true : false;
    }
}

