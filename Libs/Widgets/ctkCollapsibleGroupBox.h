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

#ifndef __ctkCollapsibleGroupBox_h
#define __ctkCollapsibleGroupBox_h

// Qt includes
#include <QGroupBox>

// CTK includes
#include "ctkWidgetsExport.h"

/// A QGroupBox with an arrow indicator that shows/hides the groupbox contents
/// when clicked. It responds to the slot QGroupBox::setChecked(bool) or
/// ctkCollapsibleGroupBox::setCollapsed(bool)
/// When checked is true, the groupbox is expanded
/// When checked is false, the groupbox is collapsed
class CTK_WIDGETS_EXPORT ctkCollapsibleGroupBox : public QGroupBox
{
  Q_OBJECT
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

  /// Reimplemtented for internal reasons
  virtual int heightForWidth(int w) const;
  /// Reimplemtented for internal reasons
  virtual QSize minimumSizeHint()const;
  /// Reimplemtented for internal reasons
  virtual QSize sizeHint()const;

protected slots:
  /// called when the arrow indicator is clicked
  /// users can call it programatically by calling setChecked(bool)
  virtual void expand(bool expand);

protected:
  /// reimplemented for internal reasons
  virtual void childEvent(QChildEvent*);

#if QT_VERSION < 0x040600
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
#endif
  virtual void resizeEvent(QResizeEvent*);
private:
  void init();
  /// Size of the widget for collapsing
  QSize OldSize;
  /// Maximum allowed height
  int   MaxHeight;
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
