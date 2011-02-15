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


#ifndef CTKMETATYPEPROVIDER_H
#define CTKMETATYPEPROVIDER_H

#include "ctkObjectClassDefinition.h"
#include <ctkPluginFrameworkExport.h>

#include <QLocale>

/**
 * \ingroup Metatype
 *
 * Provides access to metatypes. This interface can be implemented on a Managed
 * Service or Managed Service Factory as well as registered as a service. When
 * registered as a service, it must be registered with a
 * {@link #METATYPE_FACTORY_PID} or {@link #METATYPE_PID} service property (or
 * both). Any PID mentioned in either of these factories must be a valid argument
 * to the {@link #getObjectClassDefinition(const QString&, const QLocale&)} method.
 */
struct CTK_PLUGINFW_EXPORT ctkMetaTypeProvider
{

  /**
   * Service property to signal that this service has
   * {@link ctkObjectClassDefinition} objects for the given PIDs. The type of
   * this service property is <code>QString</code>.
   */
  static const QString METATYPE_PID; // = "metatype.pid"

  /**
   * Service property to signal that this service has
   * {@link ctkObjectClassDefinition} objects for the given factory PIDs. The
   * type of this service property is <code>QString</code>.
   */
  static const QString METATYPE_FACTORY_PID; // = "metatype.factory.pid"

  virtual ~ctkMetaTypeProvider() {}

  /**
   * Returns an object class definition for the specified id localized to the
   * specified locale. If no locale is specified, the default locale is used,
   * according to the default constructor of QLocale.
   *
   * @param id The ID of the requested object class. This can be a pid or
   *        factory pid returned by getPids or getFactoryPids.
   * @param locale The locale of the definition
   * @return A <code>ctkObjectClassDefinition</code> object.
   */
  virtual ctkObjectClassDefinitionPtr getObjectClassDefinition(
    const QString& id, const QLocale& locale = QLocale()) = 0;

  /**
   * Return a list of available locales.
   *
   * @return A list of QLocale objects.
   */
  virtual QList<QLocale> getLocales() const = 0;

};

Q_DECLARE_INTERFACE(ctkMetaTypeProvider, "org.commontk.service.metatype.MetaTypeProvider")

#endif // CTKMETATYPEPROVIDER_H
