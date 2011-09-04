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

#ifndef __ctkScopedCurrentDir_h
#define __ctkScopedCurrentDir_h

// Qt includes
#include <QScopedPointer>

// CTK includes
#include "ctkCoreExport.h"

class ctkScopedCurrentDirPrivate;

///
/// \brief Use this class to change the current application directory in a given scope
/// and automatically restore it.
///
/// This is particulary useful in case a plugin and its dependent libraries should be loaded from
/// a known directory.
/// Indeed, changing the application PATH, LD_LIBRARY_PATH or DYLD_LIBRARY_PATH within the current
/// process won't have the desired effect. The loader checks for these variables only once when
/// the process starts.
/// \sa http://stackoverflow.com/questions/856116/changing-ld-library-path-at-runtime-for-ctypes
/// \sa http://stackoverflow.com/questions/1178094/change-current-process-environment
///
/// \ingroup Core
class CTK_CORE_EXPORT ctkScopedCurrentDir
{
public:
  explicit ctkScopedCurrentDir(const QString& path);
  virtual ~ctkScopedCurrentDir();

  /// Return the current application path
  /// \sa QDir::currentPath
  QString currentPath()const;

  /// Return saved current path
  QString savedCurrentPath()const;
  
protected:
  QScopedPointer<ctkScopedCurrentDirPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkScopedCurrentDir);
  Q_DISABLE_COPY(ctkScopedCurrentDir);
};

#endif


