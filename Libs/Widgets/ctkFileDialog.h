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

#ifndef __ctkFileDialog_h
#define __ctkFileDialog_h

// Qt includes
#include <QAbstractItemView>
#include <QFileDialog>

// CTK includes
#include <ctkPimpl.h>
#include "ctkWidgetsExport.h"

class ctkFileDialogPrivate;

/// \ingroup Widgets
/// Customizable QFileDialog.
/// An extra widget can be added at the bottom of the dialog
/// under the file format combobox. The Accept button is also controllable
/// using setAcceptButtonEnable().
/// The behavior of the "return" key is the following:
///  - it selects the directory written in the line edit or it
///  - it accepts the dialog if the directory is already selected.
class CTK_WIDGETS_EXPORT ctkFileDialog : public QFileDialog
{
  Q_OBJECT
  Q_PROPERTY(QAbstractItemView::SelectionMode SelectionMode READ selectionMode WRITE setSelectionMode)

public:
  // Superclass typedef
  typedef QFileDialog Superclass;
  /// Constructor
  /// By default, behaves like a QFileDialog
  /// \sa QFileDialog()
  explicit ctkFileDialog(QWidget *parent = 0,
              const QString &caption = QString(),
              const QString &directory = QString(),
              const QString &filter = QString());
  virtual ~ctkFileDialog();
  
  /// Add an extra widget under the file format combobox. If a label is
  /// given, it will appear in the first column.
  /// The widget is reparented to ctkFileDialog
  Q_INVOKABLE void setBottomWidget(QWidget* widget, const QString& label=QString());

  /// Return the extra widget if any
  Q_INVOKABLE QWidget* bottomWidget()const;

  /// Set the selection mode the views operate in.
  ///
  /// \warning The selection mode must explicitly be set each time
  /// QFileDialog::setFileMode(FileMode mode) is invoked. This is required
  /// because the QFileDialog::setFileMode(FileMode mode) method is not virtual
  /// and it internally resets the selection mode.
  ///
  /// \sa clearSelection()
  void setSelectionMode(QAbstractItemView::SelectionMode mode);

  /// Get the selection mode of the views.
  ///
  /// \sa setSelectionMode(QAbstractItemView::SelectionMode)
  QAbstractItemView::SelectionMode selectionMode() const;

  /// Internally used
  bool eventFilter(QObject *obj, QEvent *event);

public Q_SLOTS:
  /// Can be used to prevent the accept button to be enabled. It's typically
  /// a slot that can be connected to assure that the user doesn't accept the
  /// dialog if a value is not set in the extra bottom widget.
  void setAcceptButtonEnable(bool enable);

  /// Deselect all selected directories or files.
  void clearSelection();

Q_SIGNALS:
  /// Signals QFileDialog::file[s]Selected() are fired only when the Ok button
  /// is pressed, fileSelectionChanged(QStringList) is emitted when the
  /// selection is changed, not just when the dialog is accepted.
  void fileSelectionChanged(const QStringList& selected);

protected Q_SLOTS:
  void onSelectionChanged();

protected:
  QScopedPointer<ctkFileDialogPrivate> d_ptr;

  /// Reimplemented to override the return key behavior
  virtual void accept();

private:
  Q_DECLARE_PRIVATE(ctkFileDialog);
  Q_DISABLE_COPY(ctkFileDialog);
};

#endif
