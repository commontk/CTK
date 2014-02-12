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

#ifndef ctkXnatLoginProfile_h
#define ctkXnatLoginProfile_h

#include "ctkXNATCoreExport.h"

#include <QScopedPointer>
#include <QUrl>

class ctkXnatLoginProfilePrivate;

/**
 * @ingroup XNAT_Core
 *
 * @brief A login profile for XNAT sessions
 */
class CTK_XNAT_CORE_EXPORT ctkXnatLoginProfile
{
public:
  ctkXnatLoginProfile();
  ctkXnatLoginProfile(const ctkXnatLoginProfile& otherLoginProfile);
  ~ctkXnatLoginProfile();

  /**
   * @brief Get the name for this profile.
   * @return The profile name.
   */
  QString name() const;

  /**
   * @brief Set the name for this profile.
   * @param profileName The new profile name.
   */
  void setName(const QString& profileName);

  /**
   * @brief Get the XNAT server URL.
   * @return The XNAT server URL.
   */
  QUrl serverUrl() const;

  /**
   * @brief Set the XNAT server URL.
   * @param serverUri The new XNAT server URL.
   */
  void setServerUrl(const QUrl& serverUri);

  /**
   * @brief Get the login user name.
   * @return The user name.
   */
  QString userName() const;

  /**
   * @brief Set the login user name.
   * @param userName The new user name.
   */
  void setUserName(const QString& userName);

  /**
   * @brief Get the login password.
   * @return The password.
   */
  QString password() const;

  /**
   * @brief Set the login password.
   * @param password The new password.
   */
  void setPassword(const QString& password);

  /**
   * @brief Returns a boolean value indicating if this login profile is the default profile.
   * @return \c true if this is the default login profile, \c false
   *         otherwise.
   */
  bool isDefault() const;

  /**
   * @brief Set the default login profile to this profile.
   * @param default_ If \c true, marks this login profile as the default profile.
   */
  void setDefault(bool default_);

private:
  /// \brief d pointer of the pimpl pattern
  QScopedPointer<ctkXnatLoginProfilePrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatLoginProfile)
};

#endif
