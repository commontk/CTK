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

#ifndef __ctkCollapsibleGroupBox_h
#define __ctkCollapsibleGroupBox_h

// Qt includes
#include <QGroupBox>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkCollapsibleGroupBoxPrivate;

/// \ingroup Widgets
/// A QGroupBox with an arrow indicator that shows/hides the groupbox contents
/// when clicked. It responds to the slot QGroupBox::setChecked(bool) or
/// ctkCollapsibleGroupBox::setCollapsed(bool)
/// When checked is true, the groupbox is expanded
/// When checked is false, the groupbox is collapsed
class CTK_WIDGETS_EXPORT ctkCollapsibleGroupBox : public QGroupBox
{
  Q_OBJECT
  Q_PROPERTY(bool collapsed READ collapsed WRITE setCollapsed)

  /// This property holds the height in pixels of the contents (excludes the title)
  /// when the box is collapsed.
  /// 14px by default, it is the smallest height that fit Mac Style.
  Q_PROPERTY(int collapsedHeight READ collapsedHeight WRITE setCollapsedHeight)

public:
  ctkCollapsibleGroupBox(QWidget* parent = 0);
  ctkCollapsibleGroupBox(const QString& title, QWidget* parent = 0);
  virtual ~ctkCollapsibleGroupBox();
  
  /// Utility function to collapse the groupbox
  /// Collapse(close) the group box if collapse is true, expand(open)
  /// it otherwise.
  /// \sa QGroupBox::setChecked(bool)
  inline void setCollapsed(bool collapse);

  /// Return the collapse state of the groupbox
  /// true if the groupbox is collapsed (closed), false if it is expanded(open)
  inline bool collapsed()const;

  /// Set the height of the collapsed box. Does not include the title height.
  virtual void setCollapsedHeight(int heightInPixels);
  int collapsedHeight()const;

  /// Reimplemented for internal reasons
  /// Catch when a child widget's visibility is externally changed
  virtual bool eventFilter(QObject* child, QEvent* e);

  /// Reimplemented for internal reasons
  virtual void setVisible(bool show);
protected Q_SLOTS:
  /// called when the arrow indicator is clicked
  /// users can call it programatically by calling setChecked(bool)
  virtual void expand(bool expand);

protected:
  QScopedPointer<ctkCollapsibleGroupBoxPrivate> d_ptr;
  /// reimplemented for internal reasons
  virtual void childEvent(QChildEvent*);

#if QT_VERSION < 0x040600
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
#endif

private:
  Q_DECLARE_PRIVATE(ctkCollapsibleGroupBox);
  Q_DISABLE_COPY(ctkCollapsibleGroupBox);
};

//----------------------------------------------------------------------------
bool ctkCollapsibleGroupBox::collapsed()const
{
  return !this->isChecked();
}

//----------------------------------------------------------------------------
void ctkCollapsibleGroupBox::setCollapsed(bool collapse)
{
  this->setChecked(!collapse);
}

#endif
