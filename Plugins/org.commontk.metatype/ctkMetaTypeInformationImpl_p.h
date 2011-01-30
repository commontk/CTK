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


#ifndef CTKMETATYPEINFORMATIONIMPL_P_H
#define CTKMETATYPEINFORMATIONIMPL_P_H

#include <service/metatype/ctkMetaTypeInformation.h>

#include "ctkMetaTypeProviderImpl_p.h"

#include <QStringList>

/**
 * Implementation of ctkMetaTypeInformation
 * <p>
 * Extension of ctkMetaTypeProvider
 * <p>
 * Provides methods to:
 * <p> - getPids() get the Pids for a given QLocale
 * <p> - getFactoryPids() get the Factory Pids for a given QLocale
 * <p>
 */
class ctkMetaTypeInformationImpl : public ctkMetaTypeProviderImpl, public ctkMetaTypeInformation
{

public:

  /**
   * Constructor of class ctkMetaTypeInformationImpl.
   */
  ctkMetaTypeInformationImpl(const QSharedPointer<ctkPlugin>& plugin, ctkLogService* logger);

  /*
   * @see ctkMetaTypeInformation#getPids()
   */
  QStringList getPids() const;

  /*
   * @see ctkMetaTypeInformation#getFactoryPids()
   */
  QStringList getFactoryPids() const;

  /*
   * @see ctkMetaTypeInformation#getPlugin()
   */
  QSharedPointer<ctkPlugin> getPlugin() const;

  /*
   * @see ctkMetaTypeProviderImpl#getLocales()
   */
  ctkObjectClassDefinitionPtr getObjectClassDefinition(
    const QString& id, const QLocale& locale = QLocale());

  /*
   * @see ctkMetaTypeProviderImpl#getLocales()
   */
  QList<QLocale> getLocales() const;
};

#endif // CTKMETATYPEINFORMATIONIMPL_P_H
