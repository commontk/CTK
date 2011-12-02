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

#ifndef __ctkFittedTextBrowser_h
#define __ctkFittedTextBrowser_h

// Qt includes
#include <QTextBrowser>

// CTK includes
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
/// ctkFittedTextBrowser is a QTextBrowser that adapts its height depending
/// on its contents and the width available. It always tries to show the whole
/// contents. ctkFittedTextBrowser doesn't resize itself but acts on the
/// sizeHint, minimumSizeHint and heightForWidth. Here sizeHint() and 
/// minimumSizeHint() are the same as ctkFittedTextBrowser always try to
/// show the whole contents.
class CTK_WIDGETS_EXPORT ctkFittedTextBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  ctkFittedTextBrowser(QWidget* parent = 0);
  virtual ~ctkFittedTextBrowser();

  /// Reimplemented for internal reasons
  virtual QSize sizeHint() const;
  /// Reimplemented for internal reasons
  virtual QSize minimumSizeHint() const;
  /// Reimplemented for internal reasons
  virtual int heightForWidth(int width) const;

protected Q_SLOTS:
  void heightForWidthMayHaveChanged();

protected:
  virtual void resizeEvent(QResizeEvent* e);
};

#endif
