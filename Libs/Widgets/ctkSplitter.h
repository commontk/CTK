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

#ifndef __ctkSplitter_h
#define __ctkSplitter_h

// Qt includes
#include <QEnterEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QSplitter>
#include <QSplitterHandle>

// CTK includes
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
/// \brief Custom splitter handle with styled appearance
///
/// Provides a visually styled splitter handle with:
/// - Gray color by default
/// - Dark gray on hover
/// - Blue when dragging
/// - Rounded grab handle in the center
class CTK_WIDGETS_EXPORT ctkSplitterHandle : public QSplitterHandle
{
  Q_OBJECT

public:
  explicit ctkSplitterHandle(Qt::Orientation orientation, QSplitter* parent);
  ~ctkSplitterHandle() override = default;

protected:
  void paintEvent(QPaintEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
  void enterEvent(QEnterEvent* event) override;
#else
  void enterEvent(QEvent* event) override;
#endif
  void leaveEvent(QEvent* event) override;
  QSize sizeHint() const override;

private:
  bool IsPressed;
};

/// \ingroup Widgets
/// \brief Custom splitter with styled handles
///
/// QSplitter subclass that creates ctkSplitterHandle instances
/// for a consistent visual appearance across CTK widgets.
class CTK_WIDGETS_EXPORT ctkSplitter : public QSplitter
{
  Q_OBJECT

public:
  explicit ctkSplitter(Qt::Orientation orientation, QWidget* parent = nullptr);
  explicit ctkSplitter(QWidget* parent = nullptr);
  ~ctkSplitter() override = default;

protected:
  QSplitterHandle* createHandle() override;
};

#endif
