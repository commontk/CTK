/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#ifndef __ctkCmdLineModuleDefaultPathBuilder_h
#define __ctkCmdLineModuleDefaultPathBuilder_h

#include "ctkCommandLineModulesCoreExport.h"
#include <QStringList>
#include <QScopedPointer>

class ctkCmdLineModuleDefaultPathBuilderPrivate;

/**
 * \class ctkCmdLineModuleDefaultPathBuilder
 * \brief Builds up a list of directory paths to search for command line modules.
 *
 * Unfinished:
 *
 * <pre>
 * Implements the following basic strategy, depending on which boolean flags are on:
 * By default they are all off, as directory scanning is often time consuming.
 *
 * 1. CTK_MODULE_LOAD_PATH environment variable
 * 2. Home directory
 * 3. Home directory / cli-modules
 * 4. Current working directory
 * 5. Current working directory / cli-modules
 * 6. Application directory
 * 7. Application directory / cli-modules
 * </pre>
 *
 * \author m.clarkson@ucl.ac.uk
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDefaultPathBuilder
{

public:

  ctkCmdLineModuleDefaultPathBuilder();
  ~ctkCmdLineModuleDefaultPathBuilder();

  virtual void setLoadFromHomeDir(const bool& doLoad);

  virtual void setLoadFromCurrentDir(const bool& doLoad);

  virtual void setLoadFromApplicationDir(const bool& doLoad);

  virtual void setLoadFromCtkModuleLoadPath(const bool& doLoad);

  virtual QStringList build() const;

private:

  QScopedPointer<ctkCmdLineModuleDefaultPathBuilderPrivate> d;
  Q_DISABLE_COPY(ctkCmdLineModuleDefaultPathBuilder)
};

#endif
