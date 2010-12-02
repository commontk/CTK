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

#ifndef __ctkFontButton_h
#define __ctkFontButton_h

// Qt includes
#include <QFont>
#include <QPushButton>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkFontButtonPrivate;

class CTK_WIDGETS_EXPORT ctkFontButton: public QPushButton
{
  Q_OBJECT
  Q_PROPERTY(QFont currentFont READ currentFont WRITE setCurrentFont NOTIFY currentFontChanged USER true)

public:
  /// Constructor
  /// Creates a default ctkFontButton initialized with QApplication font
  ctkFontButton(QWidget * parent = 0);
  /// Constructor
  /// Creates a ctkFontButton with a given font
  ctkFontButton(const QFont& font, QWidget * parent = 0);
  
  /// Destructor
  virtual ~ctkFontButton();

  /// Set/get the current font
  void setCurrentFont(const QFont& newFont);
  QFont currentFont()const;

public slots:
  /// browse() opens a pop up where the user can select a new font.
  /// browse() is automatically called when the button is clicked.
  void browseFont();

signals:
  /// Fired anytime the current font changed.
  /// Programatically or by the user via the file dialog that pop up when 
  /// clicking on the button.
  void currentFontChanged(const QFont&);
protected:
  QScopedPointer<ctkFontButtonPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkFontButton);
  Q_DISABLE_COPY(ctkFontButton);
};

#endif
