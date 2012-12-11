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
#include "ctkCmdLineModuleQtUiLoader.h"

#include <QBuffer>
#include <QFile>
#include <QUiLoader>
#include <QWidget>
#include <QVariant>
#include <QCoreApplication>

#include <QDebug>

//-----------------------------------------------------------------------------
struct ctkCmdLineModuleFrontendQtGuiPrivate
{
  ctkCmdLineModuleFrontendQtGuiPrivate()
    : Widget(NULL)
  {}

  mutable QScopedPointer<QUiLoader> Loader;
  mutable QScopedPointer<QIODevice> xslFile;
  mutable QScopedPointer<ctkCmdLineModuleXslTransform> Transform;
  mutable QWidget* Widget;

  // Cache the list of parameter names
  mutable QList<QString> ParameterNames;
};

//-----------------------------------------------------------------------------
ctkCmdLineModuleFrontendQtGui::ctkCmdLineModuleFrontendQtGui(const ctkCmdLineModuleReference& moduleRef)
  : ctkCmdLineModuleFrontend(moduleRef),
    d(new ctkCmdLineModuleFrontendQtGuiPrivate)
{
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleFrontendQtGui::~ctkCmdLineModuleFrontendQtGui()
{
}


//-----------------------------------------------------------------------------
QUiLoader* ctkCmdLineModuleFrontendQtGui::uiLoader() const
{
  if (d->Loader == NULL)
  {
    d->Loader.reset(new ctkCmdLineModuleQtUiLoader());
  }
  return d->Loader.data();
}


//-----------------------------------------------------------------------------
ctkCmdLineModuleXslTransform* ctkCmdLineModuleFrontendQtGui::xslTransform() const
{
  if (d->Transform == NULL)
  {
    d->Transform.reset(new ctkCmdLineModuleXslTransform());
    d->xslFile.reset(new QFile(":/ctkCmdLineModuleXmlToQtUi.xsl"));
    d->Transform->setXslTransformation(d->xslFile.data());
  }
  return d->Transform.data();
}


//-----------------------------------------------------------------------------
QVariant ctkCmdLineModuleFrontendQtGui::customValue(const QString& parameter, const QString& propertyName) const
{
  if (!d->Widget) return QVariant();

  ctkCmdLineModuleObjectTreeWalker reader(d->Widget);
  while(reader.readNextParameter())
  {
    if(reader.name() == parameter)
    {
      return reader.value(propertyName);
    }
  }
  return QVariant();
}

//-----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGui::setCustomValue(const QString& parameter, const QVariant &value,
                                                   const QString& propertyName)
{
  if (!d->Widget) return;

  ctkCmdLineModuleObjectTreeWalker walker(d->Widget);
  while(walker.readNextParameter())
  {
    if(walker.name() == parameter && walker.value(propertyName) != value)
    {
      walker.setValue(value, propertyName);
      break;
    }
  }
}

//-----------------------------------------------------------------------------
QObject* ctkCmdLineModuleFrontendQtGui::guiHandle() const
{
  if (d->Widget) return d->Widget;

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
    uiLoader->addPluginPath(appPath + "/../designer");
  }
#endif
  d->Widget = uiLoader->load(&uiForm);
  return d->Widget;
}


//-----------------------------------------------------------------------------
QVariant ctkCmdLineModuleFrontendQtGui::value(const QString &parameter, int role) const
{
  Q_UNUSED(role)

  // This will always return data using the default property for parameter values,
  // which holds the data for the DisplayRole.
  return customValue(parameter);
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGui::setValue(const QString &parameter, const QVariant &value, int role)
{
  if (role != DisplayRole) return;

  QVariant oldValue = this->customValue(parameter);

  // This sets the value of the default QObject property for the DisplayRole.
  this->setCustomValue(parameter, value);

  // Before emitting the signal, get the actual value because it might be different
  // (Widgets with constraints on the value domain might adapt the value).
  QVariant currentValue = this->customValue(parameter);
  if (currentValue != oldValue)
  {
    emit valueChanged(parameter, currentValue);
  }
}


//-----------------------------------------------------------------------------
QList<QString> ctkCmdLineModuleFrontendQtGui::parameterNames() const
{
  if (!d->ParameterNames.empty()) return d->ParameterNames;

  // Compute the list of parameter names using the widget hierarchy
  // if it has already created (otherwise fall back to the superclass
  // implementation.
  // This avoids creating a ctkCmdLineModuleDescription instance.
  if (d->Widget == 0) return ctkCmdLineModuleFrontend::parameterNames();

  ctkCmdLineModuleObjectTreeWalker walker(d->Widget);
  while(walker.readNextParameter())
  {
    d->ParameterNames.push_back(walker.name());
  }
  return d->ParameterNames;
}


//-----------------------------------------------------------------------------
void ctkCmdLineModuleFrontendQtGui::setParameterContainerEnabled(const bool& enabled)
{
  if (d->Widget == 0) return;

  ctkCmdLineModuleObjectTreeWalker walker(d->Widget);
  while(walker.readNextParameterContainer())
  {
    QVariant value(enabled);
    walker.setValue(value, "enabled");
  }
}
