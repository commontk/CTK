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

#ifndef __ctkFontButton_h
#define __ctkFontButton_h

// Qt includes
#include <QFont>
#include <QPushButton>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkFontButtonPrivate;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkFontButton: public QPushButton
{
  Q_OBJECT

  /// Application QFont by default
  Q_PROPERTY(QFont currentFont READ currentFont WRITE setCurrentFont NOTIFY currentFontChanged USER true)

  /// This property holds the format of the text of the pushbutton. These
  /// expressions may be used in place of the current font:
  /// * fff family
  /// * sss pointSize with suffix "pt". e.g. "9pt"
  /// * ss pointSize with no suffix. e.g. "9"
  /// * www full name weight e.g. Bold (if font is bold)
  /// * ww weight. e.g. 50 (if font is normal)
  /// * biu 3 character string where the first character is 'b' if bold or - otherwise,
  /// the second character is 'i' if the font italic or - otherwise and the third
  /// characis is 'u' if the font is underline or '-' otherwise. e.g. "bi-" if the font
  /// is bold and italic but not underline
  /// * bbb 'bold' if bold, nothing otherwise
  /// * bb 'b' if bold, nothing otherwise
  /// * iii 'italic' if italic, nothing otherwise
  /// * ii 'i' if italic, nothing otherwise
  /// * uuu 'underline' if underline, nothing otherwise
  /// * uu 'u' if underline, nothing otherwise
  /// Note that the listing order matches the replacement order.
  /// fff-sss by default.
  Q_PROPERTY(QString fontTextFormat READ fontTextFormat WRITE setFontTextFormat)

public:
  /// Constructor
  /// Creates a default ctkFontButton initialized with QApplication font
  ctkFontButton(QWidget * parent = 0);

  /// Constructor
  /// Creates a ctkFontButton with a given font
  ctkFontButton(const QFont& currentFont, QWidget * parent = 0);
  
  /// Destructor
  virtual ~ctkFontButton();

  /// Set/get the current font
  void setCurrentFont(const QFont& newFont);
  QFont currentFont()const;

  /// Set the font text format
  /// \sa fontTextFormat, fontTextFormat()
  void setFontTextFormat(const QString& fontTextFormat);

  /// Get the font text format
  /// \sa fontTextFormat, setFontTextFormat()
  QString fontTextFormat()const;

public Q_SLOTS:
  /// browse() opens a pop up where the user can select a new font.
  /// browse() is automatically called when the button is clicked.
  void browseFont();

Q_SIGNALS:
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
