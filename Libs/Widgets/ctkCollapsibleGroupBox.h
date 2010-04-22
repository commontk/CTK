/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkCollapsibleGroupBox_h
#define __ctkCollapsibleGroupBox_h

// Qt includes
#include <QGroupBox>

// CTK includes
#include "CTKWidgetsExport.h"

class CTK_WIDGETS_EXPORT ctkCollapsibleGroupBox : public QGroupBox
{
  Q_OBJECT
public:
  explicit ctkCollapsibleGroupBox(QWidget* parent = 0);
  virtual ~ctkCollapsibleGroupBox();
  virtual int heightForWidth(int w) const;
  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

protected slots:
  virtual void expand(bool expand);

protected:
  virtual void childEvent(QChildEvent*);

#if QT_VERSION < 0x040600
  virtual void paintEvent(QPaintEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
#endif
  virtual void resizeEvent(QResizeEvent*);

  QSize OldSize;
  int   MaxHeight;
};

#endif
