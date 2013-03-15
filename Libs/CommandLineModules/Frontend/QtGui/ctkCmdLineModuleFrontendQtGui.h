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
 * \ingroup CommandLineModulesFrontendQtGui_API
 *
 * This class is able to generate a Qt widgets based GUI from the XML description of
 * a given module. It uses a customizable XML stylesheet to transform the raw XML description
 * into a .ui file which is fed into a QUiLoader to generate the GUI at runtime.
 *
 * Sub-classes have several possibilities to customize the generated GUI:
 * <ul>
 * <li>Override uiLoader() and provide your own QUiLoader or ctkCmdLineModuleQtUiLoader sub-class
 *     which knows how to instantiate widget types (see the table below for widget class names).</li>
 * <li>Bind variables to the ctkCmdLineModuleXslTranform object returned by xslTransform() to
 *     customize widget class names and property names. This may require you to provide a Qt Designer
 *     plug-in for your custom widget classes if you do not implement your own widget instantiation
 *     code in a custom QUiLoader.</li>
 * <li>Advanced: Override fragments of the XML stylesheet using ctkCmdLineModuleXslTranform::setXslExtraTransformation()</li>
 * </ul>
 *
 * All widget classes are assumed to expose a readable and writable QObject property for storing and
 * retrieving current front-end values via the DisplayRole role.
 *
 * The following table lists the available XSL parameters (setable via ctkCmdLineModuleXslTransform::bindVariable()),
 * and their default values for all parameter types and created container widgets:
 *
 * \htmlonly
 * <style type="text/css">
 * table.xslparam { color:#333333; border-collapse:collapse; }
 * table.xslparam td { border: 1px solid #666666; padding: 5px; }
 * table.xslparam th { border: 1px solid #666666; padding: 5px; background-color: #dedede; font-weight: bold; }
 * </style>
 * <table class="xslparam">
 * <tr><th rowspan="2">Parameter Type</th><th colspan="4" align="center">XSL Parameters</th></tr>
 * <tr><th>Widget Class</th><th>Default</th><th>Property Name (DisplayRole)</th><th>Default</th></tr>
 * <tr><td>boolean</td><td>booleanWidget</td><td>QCheckBox</td><td>booleanValueProperty</td><td>checked</td></tr>
 * <tr><td>integer</td><td>integerWidget</td><td>QSpinBox</td><td>integerValueProperty</td><td>value</td></tr>
 * <tr><td>float</td><td>floatingWidget</td><td>QDoubleSpinBox</td><td>floatValueProperty</td><td>value</td></tr>
 * <tr><td>double</td><td>floatingWidget</td><td>QDoubleSpinBox</td><td>floatValueProperty</td><td>value</td></tr>
 * <tr><td>string</td><td>vectorWidget</td><td>QLineEdit</td><td>vectorValueProperty</td><td>text</td></tr>
 * <tr><td>integer-vector</td><td>vectorWidget</td><td>QLineEdit</td><td>vectorValueProperty</td><td>text</td></tr>
 * <tr><td>float-vector</td><td>vectorWidget</td><td>QLineEdit</td><td>vectorValueProperty</td><td>value</td></tr>
 * <tr><td>double-vector</td><td>vectorWidget</td><td>QLineEdit</td><td>vectorValueProperty</td><td>value</td></tr>
 * <tr><td>string-vector</td><td>vectorWidget</td><td>QLineEdit</td><td>vectorValueProperty</td><td>value</td></tr>
 * <tr><td>integer-enumeration</td><td>enumWidget</td><td>QComboBox (ctkCmdLineModuleQtUiLoader instantiates a custom private QComboBox sub-class)</td><td>enumerationValueProperty</td><td>currentEnumeration</td></tr>
 * <tr><td>float-enumeration</td><td>enumWidget</td><td>QComboBox</td><td>enumerationValueProperty</td><td>currentEnumeration</td></tr>
 * <tr><td>double-enumeration</td><td>enumWidget</td><td>QComboBox</td><td>enumerationValueProperty</td><td>currentEnumeration</td></tr>
 * <tr><td>string-enumeration</td><td>enumWidget</td><td>QComboBox</td><td>enumerationValueProperty</td><td>currentEnumeration</td></tr>
 * <tr><td>file (input channel)</td><td>fileInputWidget</td><td>ctkPathLineEdit</td><td>fileInputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>file (output channel)</td><td>fileOutputWidget</td><td>ctkPathLineEdit</td><td>fileOutputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>geometry (input channel)</td><td>fileInputWidget</td><td>ctkPathLineEdit</td><td>geometryInputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>geometry (output channel)</td><td>fileOutputWidget</td><td>ctkPathLineEdit</td><td>geometryOutputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>directory</td><td>directoryWidget</td><td>ctkPathLineEdit</td><td>directoryValueProperty</td><td>currentPath</td></tr>
 * <tr><td>point</td><td>pointWidget</td><td>ctkCoordinatesWidget</td><td>pointValueProperty</td><td>coordinates</td></tr>
 * <tr><td>region</td><td>pointWidget</td><td>ctkCoordinatesWidget</td><td>pointValueProperty</td><td>coordinates</td></tr>
 * <tr><td>image (input channel)</td><td>imageInputWidget</td><td>ctkPathLineEdit</td><td>imageInputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>image (output channel)</td><td>imageOutputWidget</td><td>ctkPathLineEdit</td><td>imageOutputValueProperty</td><td>currentPath</td></tr>
 * <tr><td>[main container]</td><td>executableWidget</td><td>QWidget</td><td>n/a</td><td>n/a</td></tr>
 * <tr><td>[group container]</td><td>parametersWidget</td><td>ctkCollapsibleGroupBox</td><td>n/a</td><td>n/a</td></tr>
 * <tr><td>[unknown type]</td><td>unsupportedWidget</td><td>QLabel</td><td>n/a</td><td>n/a</td></tr>
 * </table>
 * \endhtmlonly
 *
 * In addition, for image and file, input and output, it is normally necessary to define a set property within
 * the generated .ui file to contain various configuration flags to pass to the widget. For example, ctkPathLineEdit
 * has a "filters" property that can be used to configure the dialog behaviour when the browse button is pressed.
 * The following XSL parameters are available to create a set property and are also setable via
 * ctkCmdLineModuleXslTransform::bindVariable(). If the empty string is bound to the Set Property Name,
 * the set property will be omitted.
 *
 * \htmlonly
 * <style type="text/css">
 * table.xslparam { color:#333333; border-collapse:collapse; }
 * table.xslparam td { border: 1px solid #666666; padding: 5px; }
 * table.xslparam th { border: 1px solid #666666; padding: 5px; background-color: #dedede; font-weight: bold; }
 * </style>
 * <table class="xslparam">
 * <tr><th rowspan="2">Parameter Type</th><th colspan="4" align="center">XSL Parameters</th></tr>
 * <tr><th>Set Property Name</th><th>Default</th><th>Set Property Value</th><th>Default</th></tr>
 * <tr><td>file (input channel)</td><td>fileInputSetProperty</td><td>filters</td><td>fileInputSetValue</td><td>ctkPathLineEdit::Files|ctkPathLineEdit::Readable</td></tr>
 * <tr><td>file (output channel)</td><td>fileOutputSetProperty</td><td>filters</td><td>fileOutputSetValue</td><td>ctkPathLineEdit::Files|ctkPathLineEdit::Writable</td></tr>
 * <tr><td>image (input channel)</td><td>imageInputSetProperty</td><td>filters</td><td>imageInputSetProperty</td><td>ctkPathLineEdit::Files|ctkPathLineEdit::Readable</td></tr>
 * <tr><td>image (output channel)</td><td>imageOutputSetProperty</td><td>filters</td><td>imageOutputSetProperty</td><td>ctkPathLineEdit::Files|ctkPathLineEdit::Writable</td></tr>
 * </table>
 * \endhtmlonly
 */
class CTK_CMDLINEMODULEQTGUI_EXPORT ctkCmdLineModuleFrontendQtGui : public ctkCmdLineModuleFrontend
{

public:

  ctkCmdLineModuleFrontendQtGui(const ctkCmdLineModuleReference& moduleRef);
  virtual ~ctkCmdLineModuleFrontendQtGui();

  // ctkCmdLineModuleFrontend overrides

  /**
   * @brief Create the actual Qt GUI.
   * @return The parent widget for the created GUI.
   *
   * The returned object is either NULL or can always be casted to QWidget*.
   */
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
   * @see ctkCmdLineModuleFrontend::setValue()
   */
  virtual void setValue(const QString& parameter, const QVariant& value, int role = DisplayRole);

  virtual QList<QString> parameterNames() const;

  /**
   * \brief There is a container QWidget within each group box, so you can
   * set the container widget to enabled/disabled, thereby affecting all
   * child widgets, without overwriting the enabled/disabled status of the
   * child widget.
   * @param enabled if true then enabled else disabled
   */
  virtual void setParameterContainerEnabled(const bool& enabled);

protected:

  /**
   * @brief Get the QUiLoader for instantiating widgets.
   * @return The QUiLoader.
   *
   * Override this method to provide your own QUiLoader sub-class for application-specific
   * customization of certain widgets.
   *
   * @see ctkCmdLineModuleQtUiLoader
   */
  virtual QUiLoader* uiLoader() const;

  /**
   * @brief Get XSL transform used to transfrom the module XML description to a Qt .ui file.
   * @return The XSL transform.
   *
   * Override this method to either customize the default transformation or completely provide
   * your own.
   *
   * @see ctkCmdLineModuleXslTransform
   */
  virtual ctkCmdLineModuleXslTransform* xslTransform() const;

  /**
   * @brief Get the value of the given parameter using the given property name.
   * @param parameter The parameter name.
   * @param propertyName The property name from which to get the value.
   * @return The parameter value for the given property name.
   *
   * If \c propertyName is empty, this method returns the value of the property used store the values
   * for the DisplayRole role.
   *
   * Sub-classes can use this method to retrieve values for custom roles.
   */
  QVariant customValue(const QString& parameter, const QString& propertyName = QString()) const;

  /**
   * @brief Set the value of the given parameter to \c value using the given property name.
   * @param parameter The parameter name.
   * @param value The new value.
   * @param propertyName The property name for which to set the value.
   *
   * If \c propertyName is empty, this method sets the value of the property used store the values
   * for the DisplayRole role.
   *
   * Sub-classes can use this method to set values for custom roles.
   */
  void setCustomValue(const QString& parameter, const QVariant& value, const QString& propertyName = QString()) ;

private:

  QScopedPointer<ctkCmdLineModuleFrontendQtGuiPrivate> d;

};

#endif // CTKCMDLINEMODULEFRONTENDQTGUI_H
