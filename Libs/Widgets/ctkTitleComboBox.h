/*=========================================================================

  Library:   CTK
 
  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 
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
