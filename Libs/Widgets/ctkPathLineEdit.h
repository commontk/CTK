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
#include <QWidget>
#include <QDir>

// CTK includes
#include "ctkWidgetsExport.h"
class ctkPathLineEditPrivate;

/** 
 * \ingroup Widgets
 * \brief Advanced line edit to select file or directory
*/
class CTK_WIDGETS_EXPORT ctkPathLineEdit: public QWidget
{
  Q_OBJECT
  Q_FLAGS(Filters)
  Q_PROPERTY ( QString label READ label WRITE setLabel )

  Q_PROPERTY ( QStringList nameFilters READ nameFilters WRITE setNameFilters)
  Q_PROPERTY ( Filters filters READ filters WRITE setFilters)
  Q_PROPERTY ( QString currentPath READ currentPath WRITE setCurrentPath USER true )
  /// Qt versions prior to 4.7.0 didn't expose QFileDialog::Options in the
  /// public API. We need to create a custom property that will be used when
  /// instanciating a QFileDialog in ctkPathLineEdit::browse()
#ifdef USE_QFILEDIALOG_OPTIONS
  Q_PROPERTY(QFileDialog::Options options READ options WRITE setOptions)
#else
  Q_PROPERTY(Options options READ options WRITE setOptions)
  Q_FLAGS(Option Options);
#endif

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

  /** Change the current extension of the edit line.
   *  If there is no extension yet, set it
  */
  void setCurrentFileExtension(const QString& extension);

Q_SIGNALS:
  /** the signal is emit when the state of hasValidInput changed
  */
  void validInputChanged(bool);

  void currentPathChanged(const QString& path);

public Q_SLOTS:
  void setCurrentPath(const QString& path);

  /** Open a QFileDialog to select a file or directory and set current text to it
   *  You would probably connect a browse push button like this:
   *  connect(myPushButton,SIGNAL(clicked()),myPathLineEdit,SLOT(browse()))
  */
  void browse();

  /** Load the history of the paths. To be restored the inputs must have been saved by
   *  saveCurrentPathInHistory().
  */

  void retrieveHistory();
  /** Save the current value (this->currentPath()) into the history. That value will be retrieved
   *  next time the retrieveHistory()
  */
  void addCurrentPathToHistory();

protected Q_SLOTS:
  void setCurrentDirectory(const QString& directory);
  void updateHasValidInput();

protected:
  QScopedPointer<ctkPathLineEditPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkPathLineEdit);
  Q_DISABLE_COPY(ctkPathLineEdit);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathLineEdit::Filters)
#ifndef USE_QFILEDIALOG_OPTIONS
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathLineEdit::Options);
#endif

#endif // __ctkPathLineEdit_h
