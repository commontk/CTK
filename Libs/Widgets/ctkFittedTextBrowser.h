/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkFittedTextBrowser_h
#define __ctkFittedTextBrowser_h

// Qt includes
#include <QTextBrowser>

// CTK includes
#include "CTKWidgetsExport.h"

class CTK_WIDGETS_EXPORT ctkFittedTextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  ctkFittedTextBrowser(QWidget* parent = 0);
  virtual ~ctkFittedTextBrowser();

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;
  virtual int heightForWidth(int width) const;

protected slots:
  void heightForWidthMayHaveChanged();

protected:
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
