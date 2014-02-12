/*=============================================================================

  Library: XNAT/Core

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

#ifndef ctkXnatSettings_h
#define ctkXnatSettings_h

#include "ctkXNATCoreExport.h"

#include <QMap>
#include <QString>

class ctkXnatLoginProfile;

/**
 * @ingroup XNAT_Core
 */
class CTK_XNAT_CORE_EXPORT ctkXnatSettings
{
public:
  virtual QString defaultDirectory() const = 0;
  virtual void setDefaultDirectory(const QString& dir) = 0;

  virtual QString defaultWorkDirectory() const = 0;
  virtual void setDefaultWorkDirectory(const QString& workDir) = 0;

  virtual QString workSubdirectory() const;

  virtual QMap<QString, ctkXnatLoginProfile*> loginProfiles() const = 0;
  virtual void setLoginProfiles(QMap<QString, ctkXnatLoginProfile*> loginProfiles) = 0;

  virtual ctkXnatLoginProfile* loginProfile(QString profileName) const = 0;
  virtual void setLoginProfile(QString profileName, ctkXnatLoginProfile*) = 0;

  virtual void removeLoginProfile(QString profileName) = 0;

  virtual ctkXnatLoginProfile* defaultLoginProfile() const = 0;

};

#endif
