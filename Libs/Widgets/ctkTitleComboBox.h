/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkTitleComboBox_h
#define __ctkTitleComboBox_h

// Qt includes
#include <QComboBox>

// CTK includes
#include "CTKWidgetsExport.h"

class CTK_WIDGETS_EXPORT ctkTitleComboBox : public QComboBox
{
  Q_OBJECT
  Q_PROPERTY(QString title READ title WRITE setTitle)
  Q_PROPERTY(QIcon icon READ icon WRITE setIcon)

public:
  explicit ctkTitleComboBox(QWidget* parent = 0);
  virtual ~ctkTitleComboBox();

  void setTitle(const QString&);
  QString title()const;
  
  void setIcon(const QIcon&);
  QIcon icon()const;

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

protected:
  virtual void paintEvent(QPaintEvent*);
  virtual QSize recomputeSizeHint(QSize &sh)const;

  QString Title;
  QIcon   Icon;
  
private:
  mutable QSize MinimumSizeHint;
  mutable QSize SizeHint;
};

#endif
