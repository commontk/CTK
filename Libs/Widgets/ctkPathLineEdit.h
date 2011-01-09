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

/** /class Advanced line edit to select file or directory
 *  /brief Widget that remember previous selection (using QSettings and objectName)
 *         the user to select a file on the disk.
*/
class CTK_WIDGETS_EXPORT ctkPathLineEdit: public QWidget
{
  Q_OBJECT
  Q_FLAGS(Filters)
  Q_PROPERTY ( QString label READ label WRITE setLabel )

  Q_PROPERTY ( QStringList nameFilters READ nameFilters WRITE setNameFilters)
  Q_PROPERTY ( Filters filters READ filters WRITE setFilters)
  Q_PROPERTY ( QString currentPath READ currentPath WRITE setCurrentPath USER true )
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

  /** Change the current extension of the edit line.
   *  If there is no extension yet, set it
  */
  void setCurrentFileExtension(const QString& extension);

signals:
  /** the signal is emit when the state of hasValidInput changed
  */
  void validInputChanged(bool);

  void currentPathChanged(const QString& path);

public slots:
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

protected slots:
  void setCurrentDirectory(const QString& directory);
  void updateHasValidInput();

protected:
  QScopedPointer<ctkPathLineEditPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkPathLineEdit);
  Q_DISABLE_COPY(ctkPathLineEdit);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkPathLineEdit::Filters)

#endif // __ctkPathLineEdit_h
