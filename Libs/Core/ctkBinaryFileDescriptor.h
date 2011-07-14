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

 Portions (c) Copyright Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See http://www.slicer.org/copyright/copyright.txt for details.

 Program:   Module Description Parser

=========================================================================*/

#ifndef __ctkBinaryFileDescriptor_h
#define __ctkBinaryFileDescriptor_h

// Qt includes
#include <QString>
#include <QScopedPointer>

#include "ctkCoreExport.h"


class ctkBinaryFileDescriptorPrivate;

/// \ingroup Core
/// Allows to resolve global symbols contained into an executable.
/// Implementation valid only for unix-like systems (Linux, Mac, ...)
class CTK_CORE_EXPORT ctkBinaryFileDescriptor
{
public:
  ctkBinaryFileDescriptor();
  ctkBinaryFileDescriptor(const QString& _fileName);
  virtual ~ctkBinaryFileDescriptor();

  QString fileName()const;
  void setFileName(const QString& _fileName);

  /// Load the object file containing the symbols
  bool load();

  /// Unload / close the object file
  bool unload();

  bool isLoaded() const;

  /// Get the address of a symbol in memory
  void* resolve(const char * symbol);

protected:
  QScopedPointer<ctkBinaryFileDescriptorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkBinaryFileDescriptor);
  Q_DISABLE_COPY(ctkBinaryFileDescriptor);

};

#endif
