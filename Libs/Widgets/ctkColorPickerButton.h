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

#ifndef __ctkColorPickerButton_h
#define __ctkColorPickerButton_h

// Qt includes
#include <QPushButton>
#include <QColor>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkColorPickerButtonPrivate;

/// \ingroup Widgets
///
/// ctkColorPickerButton is a QPushButton that refers to a color. The color
/// and the name of the color (i.e. &#35;FFFFFF) are displayed on the button.
/// When clicked, a color dialog pops up to select a new color
/// for the QPushButton.
class CTK_WIDGETS_EXPORT ctkColorPickerButton : public QPushButton
{
  Q_OBJECT
  Q_FLAGS(ColorDialogOption ColorDialogOptions)

  /// This property controls the name of the color.
  /// Black (0,0,0) by default.
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged USER true)

  /// This property controls the name of the color.
  /// If empty (default), the color in the format "#RRGGBB" is displayed in the
  /// button if \a displayColorName is true, otherwise, the color name is used.
  Q_PROPERTY(QString colorName READ colorName WRITE setColorName NOTIFY colorNameChanged)

  /// This properties controls whether the name of the color is shown on the
  /// button if true or the button text instead. True by default.
  /// \sa colorName, QPushButton::text
  Q_PROPERTY(bool displayColorName READ displayColorName WRITE setDisplayColorName DESIGNABLE true)

  /// This property controls the properties of the dialog used in \a changeColor
  Q_PROPERTY(ColorDialogOptions dialogOptions READ dialogOptions WRITE setDialogOptions)
public:
  enum ColorDialogOption {
    ShowAlphaChannel    = 0x00000001,
    NoButtons           = 0x00000002,
    DontUseNativeDialog = 0x00000004,
    UseCTKColorDialog   = 0x0000000C
  };
  Q_DECLARE_FLAGS(ColorDialogOptions, ColorDialogOption)

  /// By default, the color is black
  explicit ctkColorPickerButton(QWidget* parent = 0);

  /// By default, the color is black. The text will be shown on the button if
  /// displayColorName is false, otherwise the color name is shown.
  /// \sa QPushButton::setText
  explicit ctkColorPickerButton(const QString& text, QWidget* parent = 0 );

  /// The text will be shown on the button if
  /// displayColorName is false, otherwise the color name is shown.
  /// \sa setColor, QPushButton::setText
  explicit ctkColorPickerButton(const QColor& color, const QString & text, QWidget* parent = 0 );

  virtual ~ctkColorPickerButton();

  /// Current selected color
  QColor color()const;

  /// Current selected color name.
  /// Returns the name of the color in the format "#RRGGBB" or the string set
  /// by setColorName().
  /// \sa color(), setColorName()
  QString colorName()const;

  /// Set the current color name.
  /// This allows you to give name other than the default "#RRGGBB"
  /// Set an invalid QString to restore the default color names
  void setColorName(const QString& name);

  ///
  /// Display the color name after color selection
  bool displayColorName()const;

  ///
  /// Set the color dialog options to configure the color dialog.
  /// \sa QColorDialog::setOptions QColorDialog::ColorDialogOption
  void setDialogOptions(const ColorDialogOptions& options);
  const ColorDialogOptions& dialogOptions() const;

  ///
  /// Reimplemented to return a toolbutton sizehint when no text is displayed
  /// in the button.
  virtual QSize sizeHint()const;

public Q_SLOTS:
  ///
  /// Set a new current color without opening a dialog
  void setColor(const QColor& color);

  /// Opens a color dialog to select a new current color.
  /// If the CTK color dialog (\a UseCTKColorDialog) is used, then the color
  /// name is also set if the user selects a named color.
  /// \sa ctkColorDialog, color, colorName
  void changeColor();

  ///
  /// Toggle the display of the color name after color selection.
  /// By default, this is activated.
  void setDisplayColorName(bool displayColorName);

Q_SIGNALS:
  /// colorChanged is fired anytime a new color is set. Programatically or
  /// by the user when choosing a color from the color dialog
  void colorChanged(QColor);

  /// This signaled is fired anytime a new color name is set.
  void colorNameChanged(QString);

protected Q_SLOTS:
  void onToggled(bool change = true);

protected:
  virtual void paintEvent(QPaintEvent* event);

  QScopedPointer<ctkColorPickerButtonPrivate> d_ptr;
private :
  Q_DECLARE_PRIVATE(ctkColorPickerButton);
  Q_DISABLE_COPY(ctkColorPickerButton);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkColorPickerButton::ColorDialogOptions)

#endif
