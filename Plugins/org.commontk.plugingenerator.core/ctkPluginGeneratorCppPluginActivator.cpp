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


#include "ctkPluginGeneratorCppPluginActivator.h"
#include "ctkPluginGeneratorConstants.h"

#include <QTextStream>

const QString ctkPluginGeneratorCppPluginActivator::PLUGINACTIVATOR_START_MARKER = "pluginactivator_start";
const QString ctkPluginGeneratorCppPluginActivator::PLUGINACTIVATOR_STOP_MARKER = "pluginactivator_stop";

ctkPluginGeneratorCppPluginActivator::ctkPluginGeneratorCppPluginActivator(ctkPluginGeneratorAbstractTemplate* parent)
  : ctkPluginGeneratorCppTemplate(ctkPluginGeneratorConstants::TEMPLATE_PLUGINACTIVATOR_CPP, parent)
{

}

QString ctkPluginGeneratorCppPluginActivator::generateContent()
{
  addContent(CPP_INCLUDES_MARKER, "#include <QtPlugin>");

  QString constructorContent;
  QTextStream constructor(&constructorContent);

  QStringList constructorInit = getContent(CPP_CONSTRUCTOR_INITLIST_MARKER);
  QStringList constructorBody = getContent(CPP_CONSTRUCTOR_BODY_MARKER);
  if (!constructorInit.isEmpty() || !constructorBody.isEmpty())
  {
    constructor << this->getClassNameToken() << "::" << this->getClassNameToken() << "()\n";

    if (!constructorInit.isEmpty())
    {
      constructor << "  : ";
      int i = 1;
      foreach(QString initToken, constructorInit)
      {
        constructor << initToken;
        if (i < constructorInit.size()) constructor << ", ";
        ++i;
      }
      constructor << "\n";
    }
    constructor << "{\n";

    if (!constructorBody.isEmpty())
    {
      int i = 1;
      foreach(QString bodyToken, constructorBody)
      {
        constructor << "  " << bodyToken.replace("\n", "\n  ") << "\n";
        if (i < constructorBody.size()) constructor << "\n";
        ++i;
      }
    }
    constructor << "}";
  }

  QString destructorContent;
  QTextStream destructor(&destructorContent);

  QStringList destructorBody = getContent(CPP_DESTRUCTOR_BODY_MARKER);
  if (!destructorBody.isEmpty())
  {
    destructor << this->getClassNameToken() << "::~" << this->getClassNameToken() << "()\n"
        << "{\n";
    int i = 1;
    foreach(QString bodyToken, destructorBody)
    {
      if (!bodyToken.isEmpty())
      {
        destructor << "\n";
      }
      else
      {
        destructor << "  " << bodyToken.replace("\n", "\n  ") << "\n";
        if (i < destructorBody.size()) destructor << "\n";
      }
      ++i;
    }
    destructor << "}";
  }

  QString startMethodContent;
  QTextStream startMethod(&startMethodContent);

  startMethod << "void " << this->getClassNameToken() << "::start(ctkPluginContext* context)\n"
      << "{\n";

  QStringList startContent = getContent(PLUGINACTIVATOR_START_MARKER);
  if (!startContent.isEmpty())
  {
    int i = 1;
    foreach(QString block, startContent)
    {
      startMethod << "  " << block.replace("\n", "\n  ") << "\n";
      if (i < startContent.size()) startMethod << "\n";
      ++i;
    }
  }
  else
  {
    startMethod << "  Q_UNUSED(context)\n";
  }

  startMethod << "}";


  QString stopMethodContent;
  QTextStream stopMethod(&stopMethodContent);

  stopMethod << "void " << this->getClassNameToken() << "::stop(ctkPluginContext* context)\n"
      << "{\n";

  QStringList stopContent = getContent(PLUGINACTIVATOR_STOP_MARKER);
  if (!stopContent.isEmpty())
  {
    int i = 1;
    foreach(QString block, stopContent)
    {
      stopMethod << "  " << block.replace("\n", "\n  ") << "\n";
      if (i < stopContent.size()) stopMethod << "\n";
    }
    ++i;
  }
  else
  {
    stopMethod << "  Q_UNUSED(context)\n";
  }

  stopMethod << "}";

  addContent(CPP_METHODS_MARKER, stopMethodContent, PREPEND);
  addContent(CPP_METHODS_MARKER, startMethodContent, PREPEND);
  if (!destructorContent.isEmpty())
  {
    addContent(CPP_METHODS_MARKER, destructorContent, PREPEND);
  }
  if (!constructorContent.isEmpty())
  {
    addContent(CPP_METHODS_MARKER, constructorContent, PREPEND);
  }

  QString exportPlugin = "Q_EXPORT_PLUGIN2(" + getSymbolicName() + ", " + getClassNameToken() + ")\n";
  addContent(CPP_METHODS_MARKER, exportPlugin);

  return ctkPluginGeneratorCppTemplate::generateContent();
}

QStringList ctkPluginGeneratorCppPluginActivator::getMarkers() const
{
  QStringList markers;
  markers << PLUGINACTIVATOR_START_MARKER
      << PLUGINACTIVATOR_STOP_MARKER;
  return markers;
}
