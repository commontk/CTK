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
#include <QStylePainter>
#include <QApplication>
#include <QDebug>

// CTK includes
#include "ctkComboBox.h"

// -------------------------------------------------------------------------
class ctkComboBoxPrivate: public ctkPrivate<ctkComboBox>
{
public:
  ctkComboBoxPrivate();
  void initStyleOption(QStyleOptionComboBox* opt)const;
  QSize recomputeSizeHint(QSize &sh) const;
  QString DefaultText;
  QIcon   DefaultIcon;
  bool    ForceDefault;
  Qt::TextElideMode ElideMode;

  mutable QSize MinimumSizeHint;
  mutable QSize SizeHint;
};

// -------------------------------------------------------------------------
ctkComboBoxPrivate::ctkComboBoxPrivate()
{
  this->DefaultText = "";
  this->ForceDefault = false;
  this->ElideMode = Qt::ElideNone;
}

// -------------------------------------------------------------------------
QSize ctkComboBoxPrivate::recomputeSizeHint(QSize &sh) const
{
  CTK_P(const ctkComboBox);
  if (sh.isValid())
    {
    return sh.expandedTo(QApplication::globalStrut());
    }

  bool hasIcon = false;
  int count = p->count();
  QSize iconSize = p->iconSize();
  const QFontMetrics &fm = p->fontMetrics();

  // text width
  if (&sh == &this->SizeHint || p->minimumContentsLength() == 0)
    {
    switch (p->sizeAdjustPolicy())
      {
      case QComboBox::AdjustToContents:
      case QComboBox::AdjustToContentsOnFirstShow:
        if (count == 0 || this->ForceDefault)
          {
          sh.rwidth() = this->DefaultText.isEmpty() ?
            7 * fm.width(QLatin1Char('x')) :
            fm.boundingRect(this->DefaultText).width();
          if (!this->DefaultIcon.isNull())
            {
            hasIcon = true;
            sh.rwidth() += iconSize.width() + 4;
            }
          }
        for (int i = 0; i < count; ++i)
          {
          if (!p->itemIcon(i).isNull())
            {
            hasIcon = true;
            sh.setWidth(qMax(sh.width(), fm.boundingRect(p->itemText(i)).width() + iconSize.width() + 4));
            }
          else
            {
            sh.setWidth(qMax(sh.width(), fm.boundingRect(p->itemText(i)).width()));
            }
          }
        break;
      case QComboBox::AdjustToMinimumContentsLength:
        if ((count == 0 || this->ForceDefault) && !this->DefaultIcon.isNull())
          {
          hasIcon = true;
          }
        for (int i = 0; i < count && !hasIcon; ++i)
          {
          hasIcon = !p->itemIcon(i).isNull();
          }
        break;
      case QComboBox::AdjustToMinimumContentsLengthWithIcon:
        hasIcon = true;
        break;
      default:
        break;
      }
    }
  else // minimumsizehint is computing and minimumcontentslenght is > 0
    {
    if ((count == 0 || this->ForceDefault) && !this->DefaultIcon.isNull())
      {
      hasIcon = true;
      }

    for (int i = 0; i < count && !hasIcon; ++i)
      {
      hasIcon = !p->itemIcon(i).isNull();
      }
    }
  if (p->minimumContentsLength() > 0)
    {
    sh.setWidth(qMax(sh.width(),
                     p->minimumContentsLength() * fm.width(QLatin1Char('X'))
                     + (hasIcon ? iconSize.width() + 4 : 0)));
    }

  // height
  sh.setHeight(qMax(fm.height(), 14) + 2);
  if (hasIcon)
    {
    sh.setHeight(qMax(sh.height(), iconSize.height() + 2));
    }

  // add style and strut values
  QStyleOptionComboBox opt;
  this->initStyleOption(&opt);
  sh = p->style()->sizeFromContents(QStyle::CT_ComboBox, &opt, sh, p);
  return sh.expandedTo(QApplication::globalStrut());
}

// -------------------------------------------------------------------------
void ctkComboBoxPrivate::initStyleOption(QStyleOptionComboBox* opt)const
{
  CTK_P(const ctkComboBox);
  p->initStyleOption(opt);
  if (p->currentIndex() == -1 ||
      this->ForceDefault)
    {
    opt->currentText = this->DefaultText;
    opt->currentIcon = this->DefaultIcon;
    }
  QRect textRect = p->style()->subControlRect(
    QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxEditField, p);
  // TODO substract icon size
  opt->currentText = opt->fontMetrics.elidedText(opt->currentText,
                                                 this->ElideMode,
                                                 textRect.width());
}


// -------------------------------------------------------------------------
ctkComboBox::ctkComboBox(QWidget* _parent)
  : QComboBox(_parent)
{
  CTK_INIT_PRIVATE(ctkComboBox);
}

// -------------------------------------------------------------------------
ctkComboBox::~ctkComboBox()
{
}

// -------------------------------------------------------------------------
void ctkComboBox::setDefaultText(const QString& newDefaultText)
{
  CTK_D(ctkComboBox);
  d->DefaultText = newDefaultText;
  d->SizeHint = QSize();
  this->update();
}

// -------------------------------------------------------------------------
QString ctkComboBox::defaultText()const
{
  CTK_D(const ctkComboBox);
  return d->DefaultText;
}

// -------------------------------------------------------------------------
void ctkComboBox::setDefaultIcon(const QIcon& newIcon)
{
  CTK_D(ctkComboBox);
  d->DefaultIcon = newIcon;
  d->SizeHint = QSize();
  this->update();
}

// -------------------------------------------------------------------------
QIcon ctkComboBox::defaultIcon()const
{
  CTK_D(const ctkComboBox);
  return d->DefaultIcon;
}

// -------------------------------------------------------------------------
void ctkComboBox::forceDefault(bool newForceDefault)
{
  CTK_D(ctkComboBox);
  if (newForceDefault == d->ForceDefault)
    {
    return;
    }
  d->ForceDefault = newForceDefault;
  d->SizeHint = QSize();
  this->updateGeometry();
}

// -------------------------------------------------------------------------
void ctkComboBox::setElideMode(const Qt::TextElideMode& newMode)
{
  CTK_D(ctkComboBox);
  d->ElideMode = newMode;
  this->update();
}
// -------------------------------------------------------------------------
Qt::TextElideMode ctkComboBox::elideMode()const
{
  CTK_D(const ctkComboBox);
  return d->ElideMode;
}

// -------------------------------------------------------------------------
bool ctkComboBox::isDefaultForced()const
{
  CTK_D(const ctkComboBox);
  return d->ForceDefault;
}

// -------------------------------------------------------------------------
void ctkComboBox::paintEvent(QPaintEvent*)
{
  CTK_D(ctkComboBox);
  QStylePainter painter(this);
  painter.setPen(palette().color(QPalette::Text));

  QStyleOptionComboBox opt;
  d->initStyleOption(&opt);

  // draw the combobox frame, focusrect and selected etc.
  painter.drawComplexControl(QStyle::CC_ComboBox, opt);
  // draw the icon and text
  painter.drawControl(QStyle::CE_ComboBoxLabel, opt);
}

// -------------------------------------------------------------------------
QSize ctkComboBox::minimumSizeHint() const
{
  CTK_D(const ctkComboBox);
  return d->recomputeSizeHint(d->MinimumSizeHint);
}

// -------------------------------------------------------------------------
/*!
    \reimp

    This implementation caches the size hint to avoid resizing when
    the contents change dynamically. To invalidate the cached value
    change the \l sizeAdjustPolicy.
*/
QSize ctkComboBox::sizeHint() const
{
  CTK_D(const ctkComboBox);
  return d->recomputeSizeHint(d->SizeHint);
}
