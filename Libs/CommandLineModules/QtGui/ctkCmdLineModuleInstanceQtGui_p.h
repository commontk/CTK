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

#ifndef CTKCMDLINEMODULEINSTANCEQTGUI_H
#define CTKCMDLINEMODULEINSTANCEQTGUI_H

#include <ctkCmdLineModuleInstance.h>

class ctkCmdLineModuleReference;

class ctkCmdLineModuleInstanceQtGui : public ctkCmdLineModuleInstance
{

public:

  ctkCmdLineModuleInstanceQtGui(const ctkCmdLineModuleReference& moduleRef);

  // ctkCmdLineModuleInstance overrides

  virtual QObject* guiHandle() const;

  virtual QVariant value(const QString& parameter) const;
  virtual void setValue(const QString& parameter, const QVariant& value);

  virtual QList<QString> parameterNames() const;

private:

  mutable QWidget* WidgetTree;

  // Cache the list of parameter names
  mutable QList<QString> ParameterNames;
};

#endif // CTKCMDLINEMODULEINSTANCEQTGUI_H
