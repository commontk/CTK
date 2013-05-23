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

#include "ctkXnatLoginProfile.h"

class ctkXnatLoginProfilePrivate
{
public:
  ctkXnatLoginProfilePrivate();

  QString Name;
  QString ServerUri;
  QString UserName;
  QString Password;
  bool Default;
};

ctkXnatLoginProfilePrivate::ctkXnatLoginProfilePrivate()
{
}

ctkXnatLoginProfile::ctkXnatLoginProfile()
  : d_ptr(new ctkXnatLoginProfilePrivate())
{
  Q_D(ctkXnatLoginProfile);

  d->Default = false;
}

ctkXnatLoginProfile::ctkXnatLoginProfile(const ctkXnatLoginProfile& otherLoginProfile)
  : d_ptr(new ctkXnatLoginProfilePrivate())
{
  Q_D(ctkXnatLoginProfile);

  d->Name = otherLoginProfile.name();
  d->ServerUri = otherLoginProfile.serverUri();
  d->UserName = otherLoginProfile.userName();
  d->Password = otherLoginProfile.password();
  d->Default = false;
}

ctkXnatLoginProfile::~ctkXnatLoginProfile()
{
}

QString ctkXnatLoginProfile::name() const
{
  Q_D(const ctkXnatLoginProfile);

  return d->Name;
}

void ctkXnatLoginProfile::setName(const QString& name)
{
  Q_D(ctkXnatLoginProfile);

  d->Name = name;
}

QString ctkXnatLoginProfile::serverUri() const
{
  Q_D(const ctkXnatLoginProfile);

  return d->ServerUri;
}

void ctkXnatLoginProfile::setServerUri(const QString& serverUri)
{
  Q_D(ctkXnatLoginProfile);

  d->ServerUri = serverUri;
}

QString ctkXnatLoginProfile::userName() const
{
  Q_D(const ctkXnatLoginProfile);

  return d->UserName;
}

void ctkXnatLoginProfile::setUserName(const QString& userName)
{
  Q_D(ctkXnatLoginProfile);

  d->UserName = userName;
}

QString ctkXnatLoginProfile::password() const
{
  Q_D(const ctkXnatLoginProfile);

  return d->Password;
}

void ctkXnatLoginProfile::setPassword(const QString& password)
{
  Q_D(ctkXnatLoginProfile);

  d->Password = password;
}

bool ctkXnatLoginProfile::isDefault() const
{
  Q_D(const ctkXnatLoginProfile);

  return d->Default;
}

void ctkXnatLoginProfile::setDefault(bool default_)
{
  Q_D(ctkXnatLoginProfile);

  d->Default = default_;
}
