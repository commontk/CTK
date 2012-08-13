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

#include "ctkCmdLineModuleQtGui_p.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleXslTransform.h"
#include "ctkCmdLineModuleObjectTreeWalker_p.h"

#include <QBuffer>
#include <QUiLoader>
#include <QWidget>
#include <QVariant>
#include <QCoreApplication>

#include <QDebug>

//-----------------------------------------------------------------------------
ctkCmdLineModuleQtGui::ctkCmdLineModuleQtGui(const ctkCmdLineModuleReference& moduleRef)
  : ctkCmdLineModule(moduleRef),
    Loader(NULL),
    Transform(NULL),
    WidgetTree(NULL)
{
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleQtGui::~ctkCmdLineModuleQtGui()
{
  if (Loader != NULL)
  {
    delete Loader;
  }

  if (Transform != NULL)
  {
    delete Transform;
  }
}


//-----------------------------------------------------------------------------
QUiLoader* ctkCmdLineModuleQtGui::uiLoader() const
{
  if (Loader == NULL)
  {
    Loader = new QUiLoader();
  }
  return Loader;
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleXslTransform* ctkCmdLineModuleQtGui::xslTransform() const
{
  if (Transform == NULL)
  {
    Transform = new ctkCmdLineModuleXslTransform();
  }
  return Transform;
}


//-----------------------------------------------------------------------------
QObject* ctkCmdLineModuleQtGui::guiHandle() const
{
  if (WidgetTree) return WidgetTree;

  QBuffer input;
  input.setData(moduleReference().rawXmlDescription());

  QBuffer uiForm;
  uiForm.open(QIODevice::ReadWrite);

  ctkCmdLineModuleXslTransform* xslTransform = this->xslTransform();
  xslTransform->setInput(&input);
  xslTransform->setOutput(&uiForm);

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
  WidgetTree = uiLoader->load(&uiForm);
  return WidgetTree;
}


//-----------------------------------------------------------------------------
QVariant ctkCmdLineModuleQtGui::value(const QString &parameter) const
{
  if (!WidgetTree) return QVariant();

  ctkCmdLineModuleObjectTreeWalker reader(WidgetTree);
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
void ctkCmdLineModuleQtGui::setValue(const QString &parameter, const QVariant &value)
{
  if (!WidgetTree) return;

  ctkCmdLineModuleObjectTreeWalker walker(WidgetTree);
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
QList<QString> ctkCmdLineModuleQtGui::parameterNames() const
{
  if (!ParameterNames.empty()) return ParameterNames;

  // Compute the list of parameter names using the widget hierarchy
  // if it has already created (otherwise fall back to the superclass
  // implementation.
  // This avoids creating a ctkCmdLineModuleDescription instance.
  if (WidgetTree == 0) return ctkCmdLineModule::parameterNames();

  ctkCmdLineModuleObjectTreeWalker walker(WidgetTree);
  while(walker.readNextParameter())
  {
    ParameterNames.push_back(walker.name());
  }
  return ParameterNames;
}
