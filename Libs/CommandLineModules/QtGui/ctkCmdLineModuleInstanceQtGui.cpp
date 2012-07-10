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

#include "ctkCmdLineModuleInstanceQtGui_p.h"
#include "ctkCmdLineModuleReference.h"
#include "ctkCmdLineModuleXslTransform.h"

#include <QBuffer>
#include <QUiLoader>
#include <QWidget>
#include <QVariant>

#include <QDebug>

ctkCmdLineModuleInstanceQtGui::ctkCmdLineModuleInstanceQtGui(const ctkCmdLineModuleReference& moduleRef)
  : WidgetTree(NULL), XmlDescription(moduleRef.rawXmlDescription())
{
}

QObject* ctkCmdLineModuleInstanceQtGui::guiHandle() const
{
  if (WidgetTree) return WidgetTree;

  QBuffer input;
  input.setData(XmlDescription);

  ctkCmdLineModuleXslTransform xslTransform(&input);
  if (!xslTransform.transform())
  {
    // maybe throw an exception
    qCritical() << xslTransform.errorString();
    return 0;
  }

  QUiLoader uiLoader;
  QByteArray uiBlob;
  uiBlob.append(xslTransform.output());

  QBuffer uiForm(&uiBlob);
  return uiLoader.load(&uiForm);
}

QVariant ctkCmdLineModuleInstanceQtGui::value(const QString& parameter) const
{
  return QVariant();
}

void ctkCmdLineModuleInstanceQtGui::setValue(const QString& parameter, const QVariant& value)
{

}
