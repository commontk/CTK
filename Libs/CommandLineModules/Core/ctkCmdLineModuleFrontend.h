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

#ifndef CTKCMDLINEMODULEFRONTEND_H
#define CTKCMDLINEMODULEFRONTEND_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QObject>

template<class K, class V> class QHash;
class QUrl;

class ctkCmdLineModuleFuture;
class ctkCmdLineModuleReference;
class ctkCmdLineModuleFrontendPrivate;

/**
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleFrontend : public QObject
{
  Q_OBJECT

public:

  enum ParameterValueRole {

    /* Data returned using this role must no be of any type not supported by
       QVariant by default. For complex parameter types (like file, image,
       geometry, etc.) the data must be convertible to a QString pointing
       to a local resource.
     */
    LocalResourceRole = 0,

    /* This role can be used in custom frontends to return a QVariant
       containing for example an in-memory representation of a complex object.
       One can then either convert the in-memory representation to a local
       resource before running a module such that arbitrary backends relying on
       the LocalResourceRole can process the data. Or one creates a custom
       backend which knows how to handle QVariants returned by this role.
     */
    UserRole = 8
  };

  enum ParameterFilter {
    Input = 0x01,
    Output = 0x02,
    All = Input | Output
  };
  Q_DECLARE_FLAGS(ParameterFilters, ParameterFilter)

  ~ctkCmdLineModuleFrontend();

  virtual QObject* guiHandle() const = 0;

  virtual QVariant value(const QString& parameter, int role = LocalResourceRole) const = 0;
  virtual void setValue(const QString& parameter, const QVariant& value) = 0;

  virtual ctkCmdLineModuleFuture future() const;

  QUrl location() const;

  ctkCmdLineModuleReference moduleReference() const;

  virtual QList<QString> parameterNames() const;

  virtual QHash<QString,QVariant> values() const;
  virtual void setValues(const QHash<QString,QVariant>& values);

  bool isRunning() const;
  bool isPaused() const;

  Q_SIGNAL void valueChanged(const QString& parameter, const QVariant& value);

protected:

  ctkCmdLineModuleFrontend(const ctkCmdLineModuleReference& moduleRef);

  void setFuture(const ctkCmdLineModuleFuture& future);

private:

  Q_DISABLE_COPY(ctkCmdLineModuleFrontend)

  friend class ctkCmdLineModuleManager;
  friend class ctkCmdLineModulePrivate;

  QScopedPointer<ctkCmdLineModuleFrontendPrivate> d;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(ctkCmdLineModuleFrontend::ParameterFilters)

#endif // CTKCMDLINEMODULEFRONTEND_H
