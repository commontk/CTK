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

// CTK includes
#include "ctkSplitter.h"

// Qt includes
#include <QPen>

//------------------------------------------------------------------------------
// ctkSplitterHandle
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ctkSplitterHandle::ctkSplitterHandle(Qt::Orientation orientation, QSplitter* parent)
  : QSplitterHandle(orientation, parent)
  , IsPressed(false)
{
  setMouseTracking(true);
  setCursor(Qt::SplitVCursor);
}

//------------------------------------------------------------------------------
void ctkSplitterHandle::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  // Determine color based on hover/pressed state
  QColor splitterColor;
  if (this->IsPressed)
  {
    splitterColor = QColor(117, 167, 255); // Blue when dragging
  }
  else if (underMouse())
  {
    splitterColor = QColor(90, 90, 90); // Dark gray on hover
  }
  else
  {
    splitterColor = QColor(128, 128, 128); // Gray default
  }

  int lineWidth = (underMouse() || this->IsPressed) ? 3 : 2;
  int centerY = height() / 2;

  // Draw splitter line
  painter.setPen(QPen(splitterColor, lineWidth));
  painter.drawLine(0, centerY, width(), centerY);

  // Draw grab handle in the middle
  int handleWidth = 40;
  int handleHeight = underMouse() ? 8 : 6;
  int handleX = (width() - handleWidth) / 2;
  int handleY = centerY - handleHeight / 2;

  QRect handleRect(handleX, handleY, handleWidth, handleHeight);
  painter.setBrush(splitterColor);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(handleRect, 3, 3);
}

//------------------------------------------------------------------------------
void ctkSplitterHandle::mousePressEvent(QMouseEvent* event)
{
  this->IsPressed = true;
  update();
  QSplitterHandle::mousePressEvent(event);
}

//------------------------------------------------------------------------------
void ctkSplitterHandle::mouseReleaseEvent(QMouseEvent* event)
{
  this->IsPressed = false;
  update();
  QSplitterHandle::mouseReleaseEvent(event);
}

//------------------------------------------------------------------------------
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void ctkSplitterHandle::enterEvent(QEnterEvent* event)
#else
void ctkSplitterHandle::enterEvent(QEvent* event)
#endif
{
  update();
  QSplitterHandle::enterEvent(event);
}

//------------------------------------------------------------------------------
void ctkSplitterHandle::leaveEvent(QEvent* event)
{
  update();
  QSplitterHandle::leaveEvent(event);
}

//------------------------------------------------------------------------------
QSize ctkSplitterHandle::sizeHint() const
{
  return QSize(width(), 10); // 10px height for handle hit area
}

//------------------------------------------------------------------------------
// ctkSplitter
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ctkSplitter::ctkSplitter(Qt::Orientation orientation, QWidget* parent)
  : QSplitter(orientation, parent)
{
  setHandleWidth(10);
  setChildrenCollapsible(false);
}

//------------------------------------------------------------------------------
ctkSplitter::ctkSplitter(QWidget* parent)
  : QSplitter(parent)
{
  setHandleWidth(10);
  setChildrenCollapsible(false);
}

//------------------------------------------------------------------------------
QSplitterHandle* ctkSplitter::createHandle()
{
  return new ctkSplitterHandle(orientation(), this);
}
