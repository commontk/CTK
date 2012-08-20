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

#include "ctkCmdLineModuleFrontendQtGui.h"

#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleObjectTreeWalker_p.h"

#include <QBuffer>
#include <QFile>
#include <QUiLoader>
#include <QWidget>
#include <QVariant>
#include <QCoreApplication>

#include <QDebug>

//-----------------------------------------------------------------------------
ctkCmdLineModuleFrontendQtGui::ctkCmdLineModuleFrontendQtGui(const ctkCmdLineModuleReference& moduleRef)
  : ctkCmdLineModuleFrontend(moduleRef),
    Loader(NULL),
    Transform(NULL),
    Widget(NULL)
{
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleFrontendQtGui::~ctkCmdLineModuleFrontendQtGui()
{
  delete this->Loader;
  delete this->Transform;
}


//-----------------------------------------------------------------------------
QUiLoader* ctkCmdLineModuleFrontendQtGui::uiLoader() const
{
  if (this->Loader == NULL)
  {
    this->Loader = new QUiLoader();
  }
  return this->Loader;
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleXslTransform* ctkCmdLineModuleFrontendQtGui::xslTransform() const
{
  if (this->Transform == NULL)
  {
    this->Transform = new ctkCmdLineModuleXslTransform();
  }
  return this->Transform;
}


//-----------------------------------------------------------------------------
QObject* ctkCmdLineModuleFrontendQtGui::guiHandle() const
{
  if (Widget) return Widget;

  QBuffer input;
  input.setData(moduleReference().rawXmlDescription());

  QBuffer uiForm;
  uiForm.open(QIODevice::ReadWrite);

  ctkCmdLineModuleXslTransform* xslTransform = this->xslTransform();
  xslTransform->setInput(&input);
  xslTransform->setOutput(&uiForm);

  QFile qtGuiTransformation(":/ctkCmdLineModuleXmlToQtUi.xsl");
  xslTransform->setXslTransformation(&qtGuiTransformation);
  if (!xslTransform->transform())
  {
    // maybe throw an exception
    qCritical() << xslTransform->errorString();
    return 0;
  }

  QUiLoader* uiLoader = this->uiLoader();
#ifdef CMAKE_INTDIR
  QString appPath = QCoreApplication::applicationDirPath();
  if (appPath.endsWith(CMAKE_INTDIR))
  {
    uiLoader.addPluginPath(appPath + "/../designer");
  }
#endif
  this->Widget = uiLoader->load(&uiForm);
  return this->Widget;
}


//-----------------------------------------------------------------------------
QVariant ctkCmdLineModuleFrontendQtGui::value(const QString &parameter) const
{
  if (!this->Widget) return QVariant();

  ctkCmdLineModuleObjectTreeWalker reader(this->Widget);
  while(reader.readNextParameter())
  {
    if(reader.name() == parameter)
    {
      return reader.value();
    }
  }
  return QVariant();
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGui::setValue(const QString &parameter, const QVariant &value)
{
  if (!this->Widget) return;

  ctkCmdLineModuleObjectTreeWalker walker(this->Widget);
  while(walker.readNextParameter())
  {
    if(walker.name() == parameter && walker.value() != value)
    {
      walker.setValue(value);
      emit valueChanged(parameter, value);
    }
  }
}


//-----------------------------------------------------------------------------
QList<QString> ctkCmdLineModuleFrontendQtGui::parameterNames() const
{
  if (!ParameterNames.empty()) return ParameterNames;

  // Compute the list of parameter names using the widget hierarchy
  // if it has already created (otherwise fall back to the superclass
  // implementation.
  // This avoids creating a ctkCmdLineModuleDescription instance.
  if (this->Widget == 0) return ctkCmdLineModuleFrontend::parameterNames();

  ctkCmdLineModuleObjectTreeWalker walker(this->Widget);
  while(walker.readNextParameter())
  {
    ParameterNames.push_back(walker.name());
  }
  return ParameterNames;
}
