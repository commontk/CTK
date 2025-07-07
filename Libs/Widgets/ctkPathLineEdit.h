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
/*=========================================================================

Program:   Maverick
Module:    $RCSfile: config.h,v $

Copyright (c) Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/

#ifndef __ctkPathLineEdit_h
#define __ctkPathLineEdit_h

// Qt includes
#include <QDir>
#include <QWidget>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QFileSystemModel>
#include <QToolButton>


// CTK includes
#include "ctkWidgetsExport.h"

/// \ingroup Widgets
/// \brief Advanced line edit to select a file or directory.
/// \sa ctkDirectoryButton, ctkPathListWidget
class ctkPathLineEditPrivate; //Forward declaration needed within file
class CTK_WIDGETS_EXPORT ctkPathLineEdit: public QWidget
{
  Q_OBJECT

  Q_PROPERTY(QString label READ label WRITE setLabel)

  Q_PROPERTY(Filters filters READ filters WRITE setFilters)
  Q_FLAGS(Filters)

  Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath USER true)

  /// Qt versions prior to 4.7.0 didn't expose QFileDialog::Options in the
  /// public API. We need to create a custom property that will be used when
  /// instantiating a QFileDialog in ctkPathLineEdit::browse()
#ifdef USE_QFILEDIALOG_OPTIONS
  Q_PROPERTY(QFileDialog::Options options READ options WRITE setOptions)
#else
  Q_PROPERTY(Options options READ options WRITE setOptions)
  Q_FLAGS(Option Options)
#endif

  /// This property controls the key used to search the settings for recorded
  /// paths.
  /// If multiple path line edits share the same key, their history is then
  /// shared.
  /// If an empty key string is given, the object name is used as key.
  /// Setting the key automatically retrieve the history from settings
  /// Empty by default.
  /// \sa retrieveHistory(), addCurrentPathToHistory(), showHistoryButton
  Q_PROPERTY(QString settingKey READ settingKey WRITE setSettingKey )

  /// This property controls whether the browse ("...") button is visible or
  /// not. Clicking on the button calls opens a dialog to select the current path.
  /// True by default
  /// \sa browse()
  Q_PROPERTY(bool showBrowseButton READ showBrowseButton WRITE setShowBrowseButton)

  /// This property controls whether the history button (arrow button that opens
  /// the history menu) is visible or not.
  /// True by default.
  /// \sa retrieveHistory(), addCurrentPathToHistory(), settingKey
  Q_PROPERTY(bool showHistoryButton READ showHistoryButton WRITE setShowHistoryButton)

  /// This property holds the policy describing how the size of the path line edit widget
  /// changes when the content changes.
  /// The default value is AdjustToMinimumContentsLength to prevent displaying
  /// of a long path making the layout too wide.
  Q_PROPERTY(SizeAdjustPolicy sizeAdjustPolicy READ sizeAdjustPolicy WRITE setSizeAdjustPolicy)

  /// This property holds the minimum number of characters that should fit into
  /// the path line edit.
  /// The default value is 0.
  /// If this property is set to a positive value, the minimumSizeHint() and sizeHint() take it into account.
  Q_PROPERTY(int minimumContentsLength READ minimumContentsLength WRITE setMinimumContentsLength)

  /// This property holds the list of regular expressions (in wildcard mode) used to help the user
  /// complete a line.
  /// For example: "Images (*.jpg *.gif *.png)"
  Q_PROPERTY(QStringList nameFilters READ nameFilters WRITE setNameFilters)

public:
  enum Filter { Dirs        = 0x001,
                Files       = 0x002,
                Drives      = 0x004,
                NoSymLinks  = 0x008,
                AllEntries  = Dirs | Files | Drives,
                TypeMask    = 0x00f,
                Readable    = 0x010,
                Writable    = 0x020,
                Executable  = 0x040,
                PermissionMask    = 0x070,
                Modified    = 0x080,
                Hidden      = 0x100,
                System      = 0x200,
                AccessMask  = 0x3F0,
                AllDirs       = 0x400,
                CaseSensitive = 0x800,
                NoDotAndDotDot = 0x1000, // ### Qt5 NoDotAndDotDot = NoDot|NoDotDot
                NoDot         = 0x2000,
                NoDotDot      = 0x4000,
                NoFilter = -1
  };
  Q_DECLARE_FLAGS(Filters, Filter)

#ifndef USE_QFILEDIALOG_OPTIONS
  // Same options than QFileDialog::Options
  enum Option
  {
    ShowDirsOnly          = 0x00000001,
    DontResolveSymlinks   = 0x00000002,
    DontConfirmOverwrite  = 0x00000004,
    DontUseSheet          = 0x00000008,
    DontUseNativeDialog   = 0x00000010,
    ReadOnly              = 0x00000020,
    HideNameFilterDetails = 0x00000040
  };
  Q_DECLARE_FLAGS(Options, Option)
#endif

  enum SizeAdjustPolicy
  {
    /// The path line edit will always adjust to the contents.
    AdjustToContents,
    /// The path line edit will adjust to its contents the first time it is shown.
    AdjustToContentsOnFirstShow,
    /// The combobox will adjust to minimumContentsLength. For performance reasons
    /// use this policy on large models.
    AdjustToMinimumContentsLength
  };
  Q_ENUM(SizeAdjustPolicy)
  /** Default constructor
  */
  ctkPathLineEdit(QWidget *parent = 0);

  /** Constructor
   *  /param label        Used in file dialogs
   *  /param nameFilters  Regular expression (in wildcard mode) used to help the user to complete the line,
   *                      example: "Images (*.jpg *.gif *.png)"
   *  /param parent       Parent widget
  */
  ctkPathLineEdit( const QString& label,
                   const QStringList& nameFilters,
                   Filters filters = ctkPathLineEdit::AllEntries,
                   QWidget *parent=0 );
  virtual ~ctkPathLineEdit();
  QString currentPath()const;

  void setLabel(const QString &label);
  const QString& label()const;

  void setNameFilters(const QStringList &nameFilters);
  const QStringList& nameFilters()const;

  void setFilters(const Filters& filters);
  Filters filters()const;

  /// Options of the file dialog pop up.
  /// \sa QFileDialog::getExistingDirectory
#ifdef USE_QFILEDIALOG_OPTIONS
  void setOptions(const QFileDialog::Options& options);
  const QFileDialog::Options& options()const;
#else
  void setOptions(const Options& options);
  const Options& options()const;
#endif

  /// Change the current extension of the edit line.
  ///  If there is no extension yet, set it
  void setCurrentFileExtension(const QString& extension);

  QString settingKey()const;
  void setSettingKey(const QString& key);

  bool showBrowseButton()const;
  void setShowBrowseButton(bool visible);

  bool showHistoryButton()const;
  void setShowHistoryButton(bool visible);

  /// the policy describing how the size of the combobox changes
  /// when the content changes
  ///
  /// The default value is \c AdjustToContentsOnFirstShow.
  ///
  /// \sa SizeAdjustPolicy
  SizeAdjustPolicy sizeAdjustPolicy() const;

  void setSizeAdjustPolicy(SizeAdjustPolicy policy);

  int minimumContentsLength()const;
  void setMinimumContentsLength(int length);

  /// Return the combo box internally used by the path line edit
  Q_INVOKABLE QComboBox* comboBox() const;

  /// The width returned, in pixels, is the length of the file name (with no
  /// path) if any. Otherwise, it's enough for 15 to 20 characters.
  virtual QSize minimumSizeHint()const;

  /// The width returned, in pixels, is the entire length of the current path
  /// if any. Otherwise, it's enough for 15 to 20 characters.
  virtual QSize sizeHint()const;

Q_SIGNALS:
  /** the signal is emit when the state of hasValidInput changed
  */
  void validInputChanged(bool);

  void currentPathChanged(const QString& path);

public Q_SLOTS:
  void setCurrentPath(const QString& path);

  /// Open a QFileDialog to select a file or directory and set current text to it.
  /// Type of dialog (file open, file save, select directory) is controlled by options flags
  /// Files (chooses between file and directory dialog) and Writable (chooses between open and save).
  /// You would probably connect a browse push button like this:
  /// connect(myPushButton,SIGNAL(clicked()),myPathLineEdit,SLOT(browse()))
  /// As a conveniency, such button is provided by default via the browseButton
  /// \sa showBrowseButton
  void browse();

  /// Load the history of the paths that have been saved in the application
  /// settings with addCurrentPathToHistory().
  /// The history is identified using \a settingKey
  /// \sa addCurrentPathToHistory(), showHistoryButton, settingKey
  void retrieveHistory();

  /// Save the current value (this->currentPath()) into the history. That value
  ///  will be retrieved next time retrieveHistory() is called.
  /// \sa retrieveHistory(), showHistoryButton, settingKey
  void addCurrentPathToHistory();

protected Q_SLOTS:
  void setCurrentDirectory(const QString& directory);
  void updateHasValidInput();

protected:
  QScopedPointer<ctkPathLineEditPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkPathLineEdit);
  Q_DISABLE_COPY(ctkPathLineEdit);

  Q_PRIVATE_SLOT(d_ptr, void _q_recomputeCompleterPopupSize())
};

//-----------------------------------------------------------------------------
/// Completer class with built-in file system model
class ctkFileCompleter : public QCompleter {
  Q_OBJECT
public:
  ctkFileCompleter(QObject* o, bool showFiles);

  // Ensure auto-completed file always uses forward-slash as separator
  QString pathFromIndex(const QModelIndex& idx) const override;

  // Helper function for getting the current model casted to QFileSystemModel
  QFileSystemModel* fileSystemModel() const;

  // Adds path to the file system model.
  // This also automatically adds all children to the model.
  void addPathToIndex(const QString& path);

  // Switch between showing files or folders only
  void setShowFiles(bool show);
  bool showFiles();

  // Set name filter. If filters is empty then all folder/file names are displayed
  // and the global shared file system models are used. If name filters are set then
  // a custom custom file system is created for the widget.
  void setNameFilters(const QStringList& filters);

  // Since nameFilters() function may be relevant when more work will be done,
  // it is commented to quiet the "-Wunused-function" warning.
  //
  // QStringList nameFilters() const;

protected:
  QFileSystemModel* CustomFileSystemModel;
};

//-----------------------------------------------------------------------------
class ctkPathLineEditPrivate
{
  Q_DECLARE_PUBLIC(ctkPathLineEdit);

protected:
  ctkPathLineEdit* const q_ptr;

public:
  ctkPathLineEditPrivate(ctkPathLineEdit& object);
  void init();
  QSize recomputeSizeHint(QSize& sh)const;
  void updateFilter();

  void adjustPathLineEditSize();

  void _q_recomputeCompleterPopupSize();

  void createPathLineEditWidget(bool useComboBox);
  QString settingKey()const;

  QLineEdit*            LineEdit;
  QComboBox*            ComboBox;
  QToolButton*          BrowseButton;       //!< "..." button

  int                   MinimumContentsLength;
  ctkPathLineEdit::SizeAdjustPolicy SizeAdjustPolicy;

  QString               Label;              //!< used in file dialogs
  QStringList           NameFilters;        //!< Regular expression (in wildcard mode) used to help the user to complete the line
  QDir::Filters         Filters;            //!< Type of path (file, dir...)
#ifdef USE_QFILEDIALOG_OPTIONS
  QFileDialog::Options DialogOptions;
#else
  ctkPathLineEdit::Options DialogOptions;
#endif

  bool                  HasValidInput;      //!< boolean that stores the old state of valid input
  QString               SettingKey;

  static QString        sCurrentDirectory;   //!< Content the last value of the current directory
  static int            sMaxHistory;     //!< Size of the history, if the history is full and a new value is added, the oldest value is dropped

  mutable QSize SizeHint;
  mutable QSize MinimumSizeHint;

  ctkFileCompleter* Completer;
  QRegExpValidator* Validator;
};


Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathLineEdit::Filters)
#ifndef USE_QFILEDIALOG_OPTIONS
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathLineEdit::Options);
#endif

#endif // __ctkPathLineEdit_h
