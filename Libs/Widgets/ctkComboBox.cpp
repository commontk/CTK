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
#include <QAbstractScrollArea>
#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QStylePainter>
#include <QWheelEvent>

// CTK includes
#include "ctkComboBox.h"

// -------------------------------------------------------------------------
class ctkComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkComboBox);
protected:
  ctkComboBox* const q_ptr;
public:
  ctkComboBoxPrivate(ctkComboBox& object);
  void initStyleOption(QStyleOptionComboBox* opt)const;
  QSize recomputeSizeHint(QSize &sh) const;
  QString DefaultText;
  QIcon   DefaultIcon;
  bool    ForceDefault;
  Qt::TextElideMode ElideMode;
  ctkComboBox::ScrollEffect ScrollWheelEffect;

  mutable QSize MinimumSizeHint;
  mutable QSize SizeHint;
};

// -------------------------------------------------------------------------
ctkComboBoxPrivate::ctkComboBoxPrivate(ctkComboBox& object)
  :q_ptr(&object)
{
  this->DefaultText = "";
  this->ForceDefault = false;
  this->ElideMode = Qt::ElideNone;
  this->ScrollWheelEffect = ctkComboBox::AlwaysScroll;
}

// -------------------------------------------------------------------------
QSize ctkComboBoxPrivate::recomputeSizeHint(QSize &sh) const
{
  Q_Q(const ctkComboBox);
  if (sh.isValid())
    {
    return sh.expandedTo(QApplication::globalStrut());
    }

  bool hasIcon = false;
  int count = q->count();
  QSize iconSize = q->iconSize();
  const QFontMetrics &fm = q->fontMetrics();

  // text width
  if (&sh == &this->SizeHint || q->minimumContentsLength() == 0)
    {
    switch (q->sizeAdjustPolicy())
      {
      case QComboBox::AdjustToContents:
      case QComboBox::AdjustToContentsOnFirstShow:
        if (count == 0 || this->ForceDefault)
          {
          #if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
          int character_pixel_width = fm.horizontalAdvance(QLatin1Char('x'));
          #else
          int character_pixel_width = fm.width(QLatin1Char('x'));
          #endif
          sh.rwidth() = this->DefaultText.isEmpty() ?
            7 * character_pixel_width :
            fm.boundingRect(this->DefaultText).width();
          if (!this->DefaultIcon.isNull())
            {
            hasIcon = true;
            sh.rwidth() += iconSize.width() + 4;
            }
          }
        for (int i = 0; i < count; ++i)
          {
          if (!q->itemIcon(i).isNull())
            {
            hasIcon = true;
            sh.setWidth(qMax(sh.width(), fm.boundingRect(q->itemText(i)).width() + iconSize.width() + 4));
            }
          else
            {
            sh.setWidth(qMax(sh.width(), fm.boundingRect(q->itemText(i)).width()));
            }
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
      hasIcon = !q->itemIcon(i).isNull();
      }
    }
  if (q->minimumContentsLength() > 0)
    {
    #if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
    int character_pixel_width = fm.horizontalAdvance(QLatin1Char('x'));
    #else
    int character_pixel_width = fm.width(QLatin1Char('x'));
    #endif
    sh.setWidth(qMax(sh.width(),
                     q->minimumContentsLength() * character_pixel_width
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
  sh = q->style()->sizeFromContents(QStyle::CT_ComboBox, &opt, sh, q);
  return sh.expandedTo(QApplication::globalStrut());
}

// -------------------------------------------------------------------------
void ctkComboBoxPrivate::initStyleOption(QStyleOptionComboBox* opt)const
{
  Q_Q(const ctkComboBox);
  q->initStyleOption(opt);
  if (q->currentIndex() == -1 ||
      this->ForceDefault)
    {
    opt->currentText = this->DefaultText;
    opt->currentIcon = this->DefaultIcon;
    }
  QRect textRect = q->style()->subControlRect(
    QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxEditField, q);
  // TODO substract icon size
  opt->currentText = opt->fontMetrics.elidedText(opt->currentText,
                                                 this->ElideMode,
                                                 textRect.width());
}


// -------------------------------------------------------------------------
ctkComboBox::ctkComboBox(QWidget* _parent)
  : QComboBox(_parent)
  , d_ptr(new ctkComboBoxPrivate(*this))
{
}

// -------------------------------------------------------------------------
ctkComboBox::~ctkComboBox()
{
}

// -------------------------------------------------------------------------
void ctkComboBox::setDefaultText(const QString& newDefaultText)
{
  Q_D(ctkComboBox);
  d->DefaultText = newDefaultText;
  d->SizeHint = QSize();
  this->update();
}

// -------------------------------------------------------------------------
QString ctkComboBox::defaultText()const
{
  Q_D(const ctkComboBox);
  return d->DefaultText;
}

// -------------------------------------------------------------------------
void ctkComboBox::setDefaultIcon(const QIcon& newIcon)
{
  Q_D(ctkComboBox);
  d->DefaultIcon = newIcon;
  d->SizeHint = QSize();
  this->update();
}

// -------------------------------------------------------------------------
QIcon ctkComboBox::defaultIcon()const
{
  Q_D(const ctkComboBox);
  return d->DefaultIcon;
}

// -------------------------------------------------------------------------
void ctkComboBox::forceDefault(bool newForceDefault)
{
  Q_D(ctkComboBox);
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
  Q_D(ctkComboBox);
  d->ElideMode = newMode;
  this->update();
}
// -------------------------------------------------------------------------
Qt::TextElideMode ctkComboBox::elideMode()const
{
  Q_D(const ctkComboBox);
  return d->ElideMode;
}

// -------------------------------------------------------------------------
bool ctkComboBox::isDefaultForced()const
{
  Q_D(const ctkComboBox);
  return d->ForceDefault;
}

// -------------------------------------------------------------------------
ctkComboBox::ScrollEffect ctkComboBox::scrollWheelEffect()const
{
  Q_D(const ctkComboBox);
  return d->ScrollWheelEffect;
}

// -------------------------------------------------------------------------
void ctkComboBox::setScrollWheelEffect(ctkComboBox::ScrollEffect scroll)
{
  Q_D(ctkComboBox);
  d->ScrollWheelEffect = scroll;
  this->setFocusPolicy( d->ScrollWheelEffect == ctkComboBox::ScrollWithFocus ?
                        Qt::StrongFocus : Qt::WheelFocus );
}

// -------------------------------------------------------------------------
void ctkComboBox::paintEvent(QPaintEvent*)
{
  Q_D(ctkComboBox);
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
  Q_D(const ctkComboBox);
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
  Q_D(const ctkComboBox);
  return d->recomputeSizeHint(d->SizeHint);
}

// -------------------------------------------------------------------------
void ctkComboBox::changeEvent(QEvent *e)
{
  Q_D(const ctkComboBox);
  switch (e->type())
    {
    case QEvent::StyleChange:
    case QEvent::MacSizeChange:
    case QEvent::FontChange:
      d->SizeHint = QSize();
      d->MinimumSizeHint = QSize();
      break;
    default:
      break;
    }

  this->QComboBox::changeEvent(e);
}

// -------------------------------------------------------------------------
void ctkComboBox::wheelEvent(QWheelEvent* event)
{
  Q_D(ctkComboBox);
  bool scroll = false;
  switch (d->ScrollWheelEffect)
    {
    case AlwaysScroll:
      scroll = true;
      break;
    case ScrollWithFocus:
      scroll = this->hasFocus();
      break;
    case ScrollWithNoVScrollBar:
      scroll = true;
      for (QWidget* ancestor = this->parentWidget();
           ancestor; ancestor = ancestor->parentWidget())
        {
        if (QAbstractScrollArea* scrollArea =
            qobject_cast<QAbstractScrollArea*>(ancestor))
          {
          scroll = !scrollArea->verticalScrollBar()->isVisible();
          if (!scroll)
            {
            break;
            }
          }
        }
      break;
    default:
    case NeverScroll:
      break;
    }
  if (scroll)
    {
    this->QComboBox::wheelEvent(event);
    }
  else
    {
    event->ignore();
    }
}

// -------------------------------------------------------------------------
QString ctkComboBox::currentUserDataAsString()const
{
  return this->itemData(this->currentIndex()).toString();
}

// -------------------------------------------------------------------------
void ctkComboBox::setCurrentUserDataAsString(QString userData)
{
  for (int index=0; index<this->count(); ++index)
    {
    QString currentItemUserData = this->itemData(index).toString();
    if (!userData.compare(currentItemUserData))
      {
      this->setCurrentIndex(index);
      return;
      }
    }

  qWarning() << Q_FUNC_INFO << ": No item found with user data string " << userData;
}
