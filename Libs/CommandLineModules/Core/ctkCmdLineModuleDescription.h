/*=============================================================================

  Library: CTK

  Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

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

#ifndef __ctkCmdLineModuleDescription_h
#define __ctkCmdLineModuleDescription_h

#include <ctkCommandLineModulesCoreExport.h>

#include <QList>
#include <QSharedDataPointer>

class QIcon;
class QIODevice;
class QTextStream;

struct ctkCmdLineModuleDescriptionPrivate;
class ctkCmdLineModuleParameterGroup;
class ctkCmdLineModuleParameter;

/**
 * \class ctkCmdLineModuleDescription
 * \brief Description of the parameters of a command line module.
 * \ingroup CommandLineModulesCore_API
 *
 * The parameters can be used for automated GUI generation or execution
 * of the module, and are directly related to the XML description used to
 * describe the command line module parameters.
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleDescription
{

public:

  ctkCmdLineModuleDescription(const ctkCmdLineModuleDescription& description);
  ~ctkCmdLineModuleDescription();

  ctkCmdLineModuleDescription& operator=(const ctkCmdLineModuleDescription& other);

  static ctkCmdLineModuleDescription parse(QIODevice* input);

  /**
   * @brief Returns the category, derived from the \code <category> \endcode tag.
   */
  QString category() const;

  /**
   * @brief Returns the title, derived from the \code <title> \endcode tag.
   */
  QString title() const;

  /**
   * @brief Helper method that returns the category followed by a dot followed by the title.
   */
  QString categoryDotTitle() const;

  /**
   * @brief Returns the title, derived from the \code <description> \endcode tag.
   */
  QString description() const;

  /**
   * @brief Returns the title, derived from the \code <version> \endcode tag.
   */
  QString version() const;

  /**
   * @brief Returns the title, derived from the \code <documentation-url> \endcode tag.
   */
  QString documentationURL() const;

  /**
   * @brief Returns the title, derived from the \code <license> \endcode tag.
   */
  QString license() const;

  /**
   * @brief Returns the title, derived from the \code <acknowledgements> \endcode tag.
   */
  QString acknowledgements() const;

  /**
   * @brief Returns the title, derived from the \code <contributor> \endcode tag.
   */
  QString contributor() const;

  /**
   * @brief Should return a QIcon, but does not appear to be supported yet.
   */
  QIcon logo() const;

  /**
   * \brief The XML can define groups of parameters, so this method returns
   * a QList of ctkCmdLineModuleParameterGroup to handle groups.
   */
  QList<ctkCmdLineModuleParameterGroup> parameterGroups() const;

  /**
   * @brief Searches the list of parameters, checking if a parameter has the given name.
   * @param name the name of the parameter, derived from the \code <name> \endcode tag.
   * @return true if this module has a parameter called name and false otherwise
   */
  bool hasParameter(const QString& name) const;

  /**
   * @brief Returns the parameter specified by name
   * @param name the name of the parameter, derived from the \code <name> \endcode tag.
   * @return the parameter
   * @throw ctkInvalidArgumentException if this module does not have this parameter.
   */
  ctkCmdLineModuleParameter parameter(const QString& name) const;

  /**
   * @brief Does the module have any simple (primitive) return types?
   */
  bool hasReturnParameters() const;

private:

  friend class ctkCmdLineModuleXmlParser;
  friend struct ctkCmdLineModuleReferencePrivate;

  ctkCmdLineModuleDescription();

  QSharedDataPointer<ctkCmdLineModuleDescriptionPrivate> d;

};

CTK_CMDLINEMODULECORE_EXPORT QTextStream & operator<<(QTextStream& os, const ctkCmdLineModuleDescription& module);

#endif
