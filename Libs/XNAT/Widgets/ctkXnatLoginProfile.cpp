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

ctkXnatLoginProfile::ctkXnatLoginProfile()
{
  m_default = false;
}

ctkXnatLoginProfile::~ctkXnatLoginProfile()
{
}

QString ctkXnatLoginProfile::name() const
{
  return m_name;
}

void ctkXnatLoginProfile::setName(const QString& name)
{
  m_name = name;
}

QString ctkXnatLoginProfile::serverUri() const
{
  return m_serverUri;
}

void ctkXnatLoginProfile::setServerUri(const QString& serverUri)
{
  m_serverUri = serverUri;
}

QString ctkXnatLoginProfile::userName() const
{
  return m_userName;
}

void ctkXnatLoginProfile::setUserName(const QString& userName)
{
  m_userName = userName;
}

QString ctkXnatLoginProfile::password() const
{
  return m_password;
}

void ctkXnatLoginProfile::setPassword(const QString& password)
{
  m_password = password;
}

bool ctkXnatLoginProfile::isDefault() const
{
  return m_default;
}

void ctkXnatLoginProfile::setDefault(const bool& default_)
{
  m_default = default_;
}
