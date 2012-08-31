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

/**
 * \class ctkCmdLineModuleFrontendQtGui
 * \brief A Qt based implementation of the module front end.
 * \ingroup CommandLineModulesFrontendQtGui
 */
class CTK_CMDLINEMODULEQTGUI_EXPORT ctkCmdLineModuleFrontendQtGui : public ctkCmdLineModuleFrontend
{

public:

  ctkCmdLineModuleFrontendQtGui(const ctkCmdLineModuleReference& moduleRef);
  virtual ~ctkCmdLineModuleFrontendQtGui();

  // ctkCmdLineModuleFrontend overrides

  virtual QObject* guiHandle() const;

  /**
   * @brief Retrieves the current parameter value using the default QObject property for
   *        parameter values.
   * @param parameter
   * @param role
   *
   * This implementation ignores the <code>role</code> argument and always returns
   * the value held by the default property, which usually correspongs to the
   * DisplayRole.
   *
   * @see ctkCmdLineModuleFrontend::value()
   */
  virtual QVariant value(const QString& parameter, int role = LocalResourceRole) const;

  /**
   * @brief Sets the parameter value.
   * @param parameter
   * @param value
   * @param role
   *
   * This implementation does nothing if the <code>role</code> parameter does not equal
   * ctkCmdLineModuleFrontend::DisplayRole. If it does, it sets the value of the default
   * QObject property to the provided value.
   *
   * @see ctkCmdLiineModuleFrontend::setValue()
   */
  virtual void setValue(const QString& parameter, const QVariant& value, int role = DisplayRole);

  virtual QList<QString> parameterNames() const;

protected:

  virtual QUiLoader* uiLoader() const;
  virtual ctkCmdLineModuleXslTransform* xslTransform() const;

  QVariant customValue(const QString& parameter, const QString& propertyName = QString()) const;
  void setCustomValue(const QString& parameter, const QVariant& value, const QString& propertyName = QString()) ;

private:

  QScopedPointer<ctkCmdLineModuleFrontendQtGuiPrivate> d;

};

#endif // CTKCMDLINEMODULEFRONTENDQTGUI_H
