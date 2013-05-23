/*=============================================================================

  Library: CTK

  Copyright (c) University College London,
    Centre for Medical Image Computing

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

#include "ctkXnatSettings.h"

#include <QDir>
#include <QFileInfo>
#include <QUuid>

#include "ctkXnatLoginProfile.h"

ctkXnatSettings::ctkXnatSettings()
{
}

ctkXnatSettings::~ctkXnatSettings()
{
}

QString ctkXnatSettings::getWorkSubdirectory() const
{
  // set work directory name
  QDir workDir;
  QString workDirName = getDefaultWorkDirectory();
  if ( !workDirName.isEmpty() )
    {
    workDir = QDir(workDirName);
    }

  // generate random name for subdirectory
  QString subdir = QUuid::createUuid().toString();

  // create subdirectory in work directory
  bool subdirCreated = workDir.mkdir(subdir);

  // check whether subdirectory was created
  if ( !subdirCreated )
    {
    // display error message
    return QString();
    }

  // return full path of subdirectory
  return QFileInfo(workDir, subdir).absoluteFilePath();
}
