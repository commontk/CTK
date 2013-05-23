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

#ifndef ctkXnatLoginProfile_h
#define ctkXnatLoginProfile_h

#include "ctkXNATWidgetsExport.h"

#include <QScopedPointer>
#include <QString>

class ctkXnatLoginProfilePrivate;

class CTK_XNAT_WIDGETS_EXPORT ctkXnatLoginProfile
{
public:
  ctkXnatLoginProfile();
  ctkXnatLoginProfile(const ctkXnatLoginProfile& otherLoginProfile);
  virtual ~ctkXnatLoginProfile();

  QString name() const;
  void setName(const QString& profileName);

  QString serverUri() const;
  void setServerUri(const QString& serverUri);

  QString userName() const;
  void setUserName(const QString& userName);

  QString password() const;
  void setPassword(const QString& password);

  bool isDefault() const;
  void setDefault(bool default_);

private:
  /// \brief d pointer of the pimpl pattern
  QScopedPointer<ctkXnatLoginProfilePrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatLoginProfile);
};

#endif
