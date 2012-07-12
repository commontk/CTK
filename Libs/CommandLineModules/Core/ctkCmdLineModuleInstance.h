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

#ifndef CTKCMDLINEMODULEINSTANCE_H
#define CTKCMDLINEMODULEINSTANCE_H

#include "ctkCommandLineModulesCoreExport.h"
#include "ctkCmdLineModuleProcessFuture.h"

#include <QObject>

class ctkCmdLineModuleReference;
class ctkCmdLineModuleInstancePrivate;

/**
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleInstance : public QObject
{
  Q_OBJECT

public:

  ~ctkCmdLineModuleInstance();

  virtual QObject* guiHandle() const = 0;

  QVariant value(const QString& parameter) const;
  void setValue(const QString& parameter, const QVariant& value);

  ctkCmdLineModuleReference moduleReference() const;

  ctkCmdLineModuleProcessFuture run() const;

  Q_SIGNAL void valueChanged(const QString& parameter, const QVariant& value);

protected:

  ctkCmdLineModuleInstance(const ctkCmdLineModuleReference& moduleRef);

  virtual QObject* parameterValueModel() const;

private:

  friend class ctkCmdLineModuleInstancePrivate;

  QScopedPointer<ctkCmdLineModuleInstancePrivate> d;

};

#endif // CTKCMDLINEMODULEINSTANCE_H
