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
