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

#ifndef CTKCMDLINEMODULEFRONTENDQTGUI_H
#define CTKCMDLINEMODULEFRONTENDQTGUI_H

#include "ctkCmdLineModuleFrontend.h"

#include "ctkCommandLineModulesFrontendQtGuiExport.h"

class ctkCmdLineModuleReference;
class ctkCmdLineModuleXslTransform;

class QUiLoader;
class QWidget;

struct ctkCmdLineModuleFrontendQtGuiPrivate;

class CTK_CMDLINEMODULEQTGUI_EXPORT ctkCmdLineModuleFrontendQtGui : public ctkCmdLineModuleFrontend
{

public:

  ctkCmdLineModuleFrontendQtGui(const ctkCmdLineModuleReference& moduleRef);
  virtual ~ctkCmdLineModuleFrontendQtGui();

  // ctkCmdLineModuleFrontend overrides

  virtual QObject* guiHandle() const;

  virtual QVariant value(const QString& parameter, int role = LocalResourceRole) const;
  virtual void setValue(const QString& parameter, const QVariant& value);

  virtual QList<QString> parameterNames() const;

protected:

  virtual QUiLoader* uiLoader() const;
  virtual ctkCmdLineModuleXslTransform* xslTransform() const;

  QVariant customValue(const QString& parameter, const QString& propertyName = QString()) const;

private:

  QScopedPointer<ctkCmdLineModuleFrontendQtGuiPrivate> d;

};

#endif // CTKCMDLINEMODULEFRONTENDQTGUI_H
