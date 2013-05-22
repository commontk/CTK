/*=============================================================================

  Plugin: org.commontk.xnat

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

class CTK_XNAT_CORE_EXPORT ctkXnatSettings
{
public:
  virtual QString getDefaultDirectory() const = 0;
  virtual void setDefaultDirectory(const QString& dir) = 0;

  virtual QString getDefaultWorkDirectory() const = 0;
  virtual void setDefaultWorkDirectory(const QString& workDir) = 0;

  virtual QString getWorkSubdirectory() const;

  virtual QMap<QString, ctkXnatLoginProfile*> getLoginProfiles() const = 0;
  virtual void setLoginProfiles(QMap<QString, ctkXnatLoginProfile*> loginProfiles) = 0;

  virtual ctkXnatLoginProfile* getLoginProfile(QString profileName) const = 0;
  virtual void setLoginProfile(QString profileName, ctkXnatLoginProfile*) = 0;

  virtual void removeLoginProfile(QString profileName) = 0;

  virtual ctkXnatLoginProfile* getDefaultLoginProfile() const = 0;

protected:
  explicit ctkXnatSettings();
  virtual ~ctkXnatSettings();
};

#endif
