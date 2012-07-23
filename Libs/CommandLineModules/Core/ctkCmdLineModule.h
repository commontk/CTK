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

#ifndef CTKCMDLINEMODULE_H
#define CTKCMDLINEMODULE_H

#include "ctkCommandLineModulesCoreExport.h"

#include <QObject>

template<class K, class V> class QHash;
template<class T> class QFuture;

class ctkCmdLineModuleResult;
typedef QFuture<ctkCmdLineModuleResult> ctkCmdLineModuleFuture;
class ctkCmdLineModuleReference;
class ctkCmdLineModulePrivate;

/**
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModule : public QObject
{
  Q_OBJECT

public:

  ~ctkCmdLineModule();

  virtual QObject* guiHandle() const = 0;

  virtual QVariant value(const QString& parameter) const = 0;
  virtual void setValue(const QString& parameter, const QVariant& value) = 0;

  virtual QList<QString> parameterNames() const;

  virtual QHash<QString,QVariant> values() const;
  virtual void setValues(const QHash<QString,QVariant>& values);

  ctkCmdLineModuleReference moduleReference() const;

  QString location() const;

  QStringList commandLineArguments() const;

  ctkCmdLineModuleFuture run() const;

  Q_SIGNAL void valueChanged(const QString& parameter, const QVariant& value);

protected:

  ctkCmdLineModule(const ctkCmdLineModuleReference& moduleRef);

private:

  friend class ctkCmdLineModulePrivate;

  QScopedPointer<ctkCmdLineModulePrivate> d;

};

#endif // CTKCMDLINEMODULE_H
