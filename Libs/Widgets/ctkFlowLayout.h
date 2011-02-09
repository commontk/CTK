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

#ifndef __ctkFlowLayout_h
#define __ctkFlowLayout_h

// Qt includes
#include <QLayout>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkFlowLayoutPrivate;

/// Warning the Vertical orientation is NOT fully supported. You can obtain
/// strange behavior
class CTK_WIDGETS_EXPORT ctkFlowLayout : public QLayout
{
  Q_OBJECT
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
  Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing)
  Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing)
  Q_PROPERTY(bool alignItems READ alignItems WRITE setAlignItems)
  Q_PROPERTY(Qt::Orientations preferredExpandingDirections READ preferredExpandingDirections WRITE setPreferredExpandingDirections)
public:
  typedef QLayout Superclass;
  explicit ctkFlowLayout(Qt::Orientation orientation, QWidget* parent = 0);
  explicit ctkFlowLayout(QWidget* parent);
  explicit ctkFlowLayout();
  virtual ~ctkFlowLayout();
  
  void setOrientation(Qt::Orientation orientation);
  Qt::Orientation orientation()const;
  
  void setPreferredExpandingDirections(Qt::Orientations directions);
  Qt::Orientations preferredExpandingDirections()const;

  int horizontalSpacing() const;
  void setHorizontalSpacing(int);

  int verticalSpacing() const;
  void setVerticalSpacing(int);
  
  bool alignItems()const;
  void setAlignItems(bool);

  virtual void addItem(QLayoutItem *item);
  virtual Qt::Orientations expandingDirections() const;
  virtual bool hasHeightForWidth() const;
  virtual int heightForWidth(int) const;
  virtual int count() const;
  virtual QLayoutItem *itemAt(int index) const;
  virtual QSize minimumSize() const;
  virtual void setGeometry(const QRect &rect);
  virtual QSize sizeHint() const;
  virtual QLayoutItem *takeAt(int index);
  
protected:
  QScopedPointer<ctkFlowLayoutPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkFlowLayout);
  Q_DISABLE_COPY(ctkFlowLayout);
};

#endif
