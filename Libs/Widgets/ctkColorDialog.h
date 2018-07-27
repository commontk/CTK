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

#ifndef __ctkColorDialog_h
#define __ctkColorDialog_h

// Qt includes
#include <QColorDialog>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkColorDialogPrivate;

/// \ingroup Widgets
/// Customizable QColorDialog.
/// Extra widgets can be added to the left of the dialog into a QStackedWidget
class CTK_WIDGETS_EXPORT ctkColorDialog : public QColorDialog
{
  Q_OBJECT
  Q_PROPERTY(QString colorName READ colorName WRITE setColorName)

public:
  /// Constructor
  /// By default, behaves like a QColorDialog
  /// \sa QColorDialog()
  explicit ctkColorDialog(QWidget* parent = 0);
  explicit ctkColorDialog(const QColor& initial, QWidget* parent = 0);
  virtual ~ctkColorDialog();

  /// Add an extra widget under the file format combobox. If a label is
  /// given, it will appear in the first column.
  /// The widget is reparented to ctkColorDialog
  /// The ownership of the widget is taken.
  /// You must manually connect the color changed signal of the widget
  /// to ctkColorDialog::setColor(QColor)
  /// Same apply if you want to specify a color name, you must connect the
  /// color name changed signal to ctkColorDialog::setColorName(QString) but
  /// you have to make sure the color name is set after setColor as it always
  /// resets the color name.
  Q_INVOKABLE inline void addTab(QWidget* widget, const QString& label);

  /// Same as addTab(), in addition, \a tabIndex control the tab index of the widget.
  /// If index is -1, the tab is appended (same as addDefaultTab). The last
  /// tab added with an index of 0 will be the first tab open
  Q_INVOKABLE void insertTab(int tabIndex, QWidget* widget, const QString& label);

  /// The ownership of widget remains the same. The widget is not deleted, 
  /// but simply removed from the widget's stacked layout, causing it to be
  /// hidden.
  Q_INVOKABLE void removeTab(int index);

  /// Set the current tab index. 0 ("Basic" tab) by default.
  Q_INVOKABLE void setCurrentTab(int index);

  /// Return the extra widget if any
  /// Be careful with the "Basic" tab.
  Q_INVOKABLE QWidget* widget(int index)const;

  /// Returns the index position of the page occupied by the widget w,
  /// or -1 if the widget cannot be found
  Q_INVOKABLE int indexOf(QWidget* widget)const;

  /// Return the current color name if any has been set.
  QString colorName()const;

  /// Pops up a modal color dialog with the given window \a title (or "Select Color" if none is
  /// specified), lets the user choose a color, and returns that color. The color is initially set
  /// to \a initial. The dialog is a child of \a parent. It returns an invalid (see
  /// QColor::isValid()) color if the user cancels the dialog.
  ///
  /// The \a options argument allows you to customize the dialog;
  /// QColorDialog::DontUseNativeDialog is forced
  Q_INVOKABLE static QColor getColor(const QColor &initial, QWidget *parent,
                         const QString &title, ColorDialogOptions options = 0);
  /// Return the last selected color name if any. getColorName() call is only
  /// valid after a getColor() call.
  /// \sa getColor
  Q_INVOKABLE static QString getColorName();

  /// Add a custom widget as an additional tab of the color dialog created by
  /// ctkColorDialog::getColor. \a label is title of the tab and \a signal is the signal fired by
  /// the widget whenever a QColor is changed, typically: SIGNAL(currentColorChanged(QColor)). It
  /// is internally connected to set the current color of the dialog
  Q_INVOKABLE static inline void addDefaultTab(QWidget* widget, const QString& label,
                                   const char* colorSignal = 0,
                                   const char* nameSignal = 0);
  /// Same as addDefaultTab, in addition, \a tabIndex control the tab index of the widget.
  /// If index is -1, the tab is appended (same as addDefaultTab). The last
  /// tab added with an index of 0 will be the first tab open
  Q_INVOKABLE static void insertDefaultTab(int tabIndex, QWidget* widget, const QString& label,
                               const char* colorSignal = 0,
                               const char* nameSignal = 0);
  /// Index of the tab to make default (active when getColor is called).
  /// -1 for the "Basic Colors", it's the default behavior
  Q_INVOKABLE static void setDefaultTab(int index);

public Q_SLOTS:
  /// Slot-ify QColorDialog::setCurrentColor(QColor)
  void setColor(const QColor& color);

  /// Set the color name.
  /// Note that each time the color is changed the name is reset.
  void setColorName(const QString& name);

Q_SIGNALS:
  void currentColorNameChanged(const QString& colorName);

protected Q_SLOTS:
  void resetColorName();

protected:
  QScopedPointer<ctkColorDialogPrivate> d_ptr;

  static QList<QWidget*> DefaultTabs;
  static int DefaultTab;
  static QString LastColorName;
private:
  Q_DECLARE_PRIVATE(ctkColorDialog);
  Q_DISABLE_COPY(ctkColorDialog);
};

//------------------------------------------------------------------------------
void ctkColorDialog::addTab(QWidget* widget, const QString& label)
{
  this->insertTab(-1, widget, label);
}

//------------------------------------------------------------------------------
void ctkColorDialog::addDefaultTab(QWidget* widget, const QString& label,
                                   const char* colorSignal,
                                   const char* nameSignal)
{
  ctkColorDialog::insertDefaultTab(-1, widget, label, colorSignal, nameSignal);
}

#endif
