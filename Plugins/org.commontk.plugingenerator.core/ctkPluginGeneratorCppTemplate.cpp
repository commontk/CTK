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


#include "ctkPluginGeneratorCppTemplate.h"

#include "ctkPluginGeneratorConstants.h"

#include <QTextStream>

const QString ctkPluginGeneratorCppTemplate::CPP_CLASSNAME_MARKER = "cpp_classname";
const QString ctkPluginGeneratorCppTemplate::CPP_INCLUDES_MARKER = "cpp_includes";
const QString ctkPluginGeneratorCppTemplate::CPP_GLOBAL_MARKER = "cpp_globals";
const QString ctkPluginGeneratorCppTemplate::CPP_METHODS_MARKER = "cpp_methods";
const QString ctkPluginGeneratorCppTemplate::CPP_CONSTRUCTOR_INITLIST_MARKER = "cpp_constructor_initlist";
const QString ctkPluginGeneratorCppTemplate::CPP_CONSTRUCTOR_BODY_MARKER = "cpp_constructor_body";
const QString ctkPluginGeneratorCppTemplate::CPP_DESTRUCTOR_BODY_MARKER = "cpp_destructor_body";

ctkPluginGeneratorCppTemplate::ctkPluginGeneratorCppTemplate(
    const QString& name, ctkPluginGeneratorAbstractTemplate* parent)
  : ctkPluginGeneratorAbstractTemplate(name, parent)
{

}

QStringList ctkPluginGeneratorCppTemplate::getMarkers() const
{
  QStringList markers = ctkPluginGeneratorAbstractTemplate::getMarkers();

  markers << CPP_CLASSNAME_MARKER
      << CPP_INCLUDES_MARKER
      << CPP_GLOBAL_MARKER
      << CPP_METHODS_MARKER
      << CPP_CONSTRUCTOR_INITLIST_MARKER
      << CPP_CONSTRUCTOR_BODY_MARKER;

  return markers;
}

QString ctkPluginGeneratorCppTemplate::generateContent()
{
  QString content;
  QTextStream stream(&content);

  // get the namespace
  QString namespaceToken;
  QStringList namespc = this->getContent(ctkPluginGeneratorConstants::PLUGIN_NAMESPACE_MARKER);
  if (!namespc.isEmpty() && !namespc.back().isEmpty())
  {
    namespaceToken = namespc.back();
  }

  // License header
  QStringList licenseText = this->getContent(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER);
  if (!licenseText.isEmpty() && !licenseText.back().isEmpty())
  {
    stream << licenseText.back() << "\n\n";
  }

  // include statements
  QStringList includes = this->getContent(CPP_INCLUDES_MARKER);
  if (!includes.isEmpty())
  {
    foreach(QString includeStatement, includes)
    {
      stream << includeStatement << "\n";
    }
    stream << "\n";
  }

  // namespace
  if (!namespaceToken.isEmpty())
  {
    stream << "namespace " << namespaceToken << " {\n\n";
  }

  // global definitions
  QStringList globals = this->getContent(CPP_GLOBAL_MARKER);
  if (!globals.isEmpty())
  {
    foreach (QString global, globals)
    {
      stream << global << "\n";
    }
    stream << "\n";
  }

  // method definitions
  QStringList methods = this->getContent(CPP_METHODS_MARKER);
  if (!methods.isEmpty())
  {
    foreach (QString method, methods)
    {
      stream << method << "\n\n";
    }
  }

  // end namespace
  if (!namespaceToken.isEmpty())
  {
    stream << "} // end namespace " << namespaceToken << "\n";
  }

  return content;
}

QString ctkPluginGeneratorCppTemplate::getClassNameToken() const
{
  QString classNameToken;
  QStringList classname = this->getContent(CPP_CLASSNAME_MARKER);
  if (!classname.isEmpty() && !classname.back().isEmpty())
  {
    classNameToken = classname.back();
  }
  else
  {
    // use the filename without ending
    classNameToken = getFilename().left(getFilename().lastIndexOf("."));
  }

  return classNameToken;
}
