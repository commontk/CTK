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


#include "ctkPluginGeneratorHeaderTemplate.h"

#include "ctkPluginGeneratorConstants.h"

#include <QTextStream>

const QString ctkPluginGeneratorHeaderTemplate::H_CLASSNAME_MARKER = "h_classname";
const QString ctkPluginGeneratorHeaderTemplate::H_INCLUDES_MARKER = "h_includes";
const QString ctkPluginGeneratorHeaderTemplate::H_FORWARD_DECL_MARKER = "h_forward_decl";
const QString ctkPluginGeneratorHeaderTemplate::H_FORWARD_DECL_NAMESPACE_MARKER = "h_forward_decl_namespace";
const QString ctkPluginGeneratorHeaderTemplate::H_SUPERCLASSES_MARKER = "h_superclasses";
const QString ctkPluginGeneratorHeaderTemplate::H_DEFAULT_ACCESS_MARKER = "h_default_access";
const QString ctkPluginGeneratorHeaderTemplate::H_PUBLIC_MARKER = "h_public";
const QString ctkPluginGeneratorHeaderTemplate::H_PROTECTED_MARKER = "h_protected";
const QString ctkPluginGeneratorHeaderTemplate::H_PRIVATE_MARKER = "h_private";

ctkPluginGeneratorHeaderTemplate::ctkPluginGeneratorHeaderTemplate(const QString& name, ctkPluginGeneratorAbstractTemplate* parent)
  : ctkPluginGeneratorAbstractTemplate(name, parent)
{

}

QStringList ctkPluginGeneratorHeaderTemplate::getMarkers() const
{
  QStringList markers = ctkPluginGeneratorAbstractTemplate::getMarkers();

  markers << H_CLASSNAME_MARKER
      << H_INCLUDES_MARKER
      << H_FORWARD_DECL_MARKER
      << H_FORWARD_DECL_NAMESPACE_MARKER
      << H_SUPERCLASSES_MARKER
      << H_DEFAULT_ACCESS_MARKER
      << H_PUBLIC_MARKER
      << H_PROTECTED_MARKER
      << H_PRIVATE_MARKER;

  return markers;
}

QString ctkPluginGeneratorHeaderTemplate::generateContent()
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

  // get the classname
  QString classNameToken;
  QStringList classname = this->getContent(H_CLASSNAME_MARKER);
  if (!classname.isEmpty() && !classname.back().isEmpty())
  {
    classNameToken = classname.back();
  }
  else
  {
    // use the filename without ending
    classNameToken = getFilename().left(getFilename().lastIndexOf("."));
  }

  // License header
  QStringList licenseText = this->getContent(ctkPluginGeneratorConstants::PLUGIN_LICENSE_MARKER);
  if (!licenseText.isEmpty() && !licenseText.back().isEmpty())
  {
    stream << licenseText.back() << "\n\n";
  }

  // include guard
  QString includeGuardToken;
  if (!namespaceToken.isEmpty())
  {
    includeGuardToken += namespaceToken.toUpper() + "_";
  }
  includeGuardToken += getFilename().toUpper().replace(".", "_");
  stream << "#ifndef " << includeGuardToken << "\n";
  stream << "#define " << includeGuardToken << "\n\n";

  // include statements
  QStringList includes = this->getContent(H_INCLUDES_MARKER);
  if (!includes.isEmpty())
  {
    foreach(QString includeStatement, includes)
    {
      stream << includeStatement << "\n";
    }
    stream << "\n";
  }

  // forward declarations
  QStringList forwards = this->getContent(H_FORWARD_DECL_MARKER);
  if (!forwards.isEmpty())
  {
    foreach(QString forward, forwards)
    {
      stream << forward << "\n";
    }
    stream << "\n";
  }

  // namespace
  if (!namespaceToken.isEmpty())
  {
    stream << "namespace " << namespaceToken << " {\n\n";
  }

  // forward declarations inside namespace
  QStringList forwardsN = this->getContent(H_FORWARD_DECL_NAMESPACE_MARKER);
  if (!forwardsN.isEmpty())
  {
    foreach(QString forward, forwardsN)
    {
      stream << forward << "\n";
    }
    stream << "\n";
  }

  // class declaration
  stream << "class ";
  QStringList exportMacro = this->getContent(ctkPluginGeneratorConstants::PLUGIN_EXPORTMACRO_MARKER);
  if (!exportMacro.isEmpty() && !exportMacro.back().isEmpty())
  {
    stream << exportMacro.back() << " ";
  }
  stream << classNameToken;
  QStringList superclasses = this->getContent(H_SUPERCLASSES_MARKER);
  if (!superclasses.isEmpty())
  {
    stream << " :\n";
    int i = 1;
    foreach(QString superclass, superclasses)
    {
      stream << "  " << superclass;
      if (i < superclasses.size()) stream << ",";
    }
  }
  stream << "\n{\n";

  // method and member declarations

  // default access
  QStringList defaultAccess = this->getContent(H_DEFAULT_ACCESS_MARKER);
  if (!defaultAccess.isEmpty())
  {
    foreach(QString entry, defaultAccess)
    {
      stream << "  " << entry.replace("\n", "\n  ") << "\n";
    }
    stream << "\n";
  }

  // public access
  QStringList publicAccess = this->getContent(H_PUBLIC_MARKER);
  if (!publicAccess.isEmpty())
  {
    stream << "public:\n\n";
    foreach(QString entry, publicAccess)
    {
      stream << "  " << entry.replace("\n", "\n  ") << "\n\n";
    }
    stream << "\n";
  }

  // protected access
  QStringList protectedAccess = this->getContent(H_PROTECTED_MARKER);
  if (!protectedAccess.isEmpty())
  {
    stream << "protected:\n\n";
    foreach(QString entry, protectedAccess)
    {
      stream << "  " << entry.replace("\n", "\n  ") << "\n\n";
    }
    stream << "\n";
  }

  // private access
  QStringList privateAccess = this->getContent(H_PRIVATE_MARKER);
  if (!privateAccess.isEmpty())
  {
    stream << "private:\n\n";
    foreach(QString entry, privateAccess)
    {
      stream << "  " << entry.replace("\n", "\n  ") << "\n\n";
    }
    stream << "\n";
  }

  // end class declaration
  stream << "}; // " << classNameToken << "\n\n";

  // end namespace
  if (!namespaceToken.isEmpty())
  {
    stream << "} // end namespace " << namespaceToken << "\n";
  }

  // end include guard
  stream << "#endif // " << includeGuardToken << "\n";

  return content;
}
