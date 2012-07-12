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

#include "ctkCmdLineModuleDefaultPathBuilder.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <cstdlib>

struct ctkCmdLineModuleDefaultPathBuilderPrivate
{
public:
  ctkCmdLineModuleDefaultPathBuilderPrivate();
  ~ctkCmdLineModuleDefaultPathBuilderPrivate();
  QStringList build();
};

//-----------------------------------------------------------------------------
// ctkCmdLineModuleDefaultPathBuilderPrivate methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilderPrivate::ctkCmdLineModuleDefaultPathBuilderPrivate()
{

}

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilderPrivate::~ctkCmdLineModuleDefaultPathBuilderPrivate()
{

}

//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDefaultPathBuilderPrivate::build()
{
  QStringList result;

  QString suffix = "cli-modules";

  char *ctkModuleLoadPath = getenv("CTK_MODULE_LOAD_PATH");
  if (ctkModuleLoadPath != NULL)
  {
    QDir dir = QDir(QString(ctkModuleLoadPath));
    if (dir.exists())
    {
      result << dir.canonicalPath();
    }
  }

  if (QDir::home().exists())
  {
    result << QDir::homePath();
    result << QDir::homePath() + QDir::separator() + suffix;
  }

  if (QDir::current().exists())
  {
    result << QDir::currentPath();
    result << QDir::currentPath() + QDir::separator() + suffix;
  }

  result << QCoreApplication::applicationDirPath();
  result << QCoreApplication::applicationDirPath() + QDir::separator() + suffix;

  return result;
}

//-----------------------------------------------------------------------------
// ctkCmdLineModuleDefaultPathBuilder methods

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilder::ctkCmdLineModuleDefaultPathBuilder()
  : d(new ctkCmdLineModuleDefaultPathBuilderPrivate)
{
}

//-----------------------------------------------------------------------------
ctkCmdLineModuleDefaultPathBuilder::~ctkCmdLineModuleDefaultPathBuilder()
{
}

//-----------------------------------------------------------------------------
QStringList ctkCmdLineModuleDefaultPathBuilder::build()
{
  return d->build();
}
