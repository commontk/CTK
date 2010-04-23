/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

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
