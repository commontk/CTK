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

#ifndef __ctkMenuButton_h
#define __ctkMenuButton_h

// Qt includes
#include <QPushButton>

// CTK includes
#include <ctkPimpl.h>

#include "CTKWidgetsExport.h"

class ctkMenuButtonPrivate;

/// Description
/// A Menu widget that show/hide its children depending on its checked/collapsed properties
class CTK_WIDGETS_EXPORT ctkMenuButton : public QPushButton
{
  Q_OBJECT

public:
  ctkMenuButton(QWidget *parent = 0);
  ctkMenuButton(const QString& text, QWidget *parent = 0);
  virtual ~ctkMenuButton();

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent* event);

  virtual bool hitButton(const QPoint & pos) const;
  virtual void initStyleOption ( QStyleOptionButton * option ) const;
private:
  CTK_DECLARE_PRIVATE(ctkMenuButton);
};

#endif
