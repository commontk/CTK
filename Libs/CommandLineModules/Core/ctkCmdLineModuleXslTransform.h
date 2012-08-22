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

#ifndef __ctkCmdLineModuleXslTransform_h
#define __ctkCmdLineModuleXslTransform_h

// CTK includes
#include "ctkCommandLineModulesCoreExport.h"
#include "ctkCmdLineModuleXmlValidator.h"
class ctkCmdLineModuleXslTransformPrivate;

// Qt includes
class QIODevice;

/**
 * \class ctkCmdLineModuleXslTransform
 * \brief Provides a transformation from XML to the Qt ui file format.
 * \ingroup CommandLineModulesCore
 */
class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleXslTransform
  : public ctkCmdLineModuleXmlValidator
{

public:

  ctkCmdLineModuleXslTransform(QIODevice* input = 0, QIODevice* output = 0);
  virtual ~ctkCmdLineModuleXslTransform();

  void setOutput(QIODevice* output);
  QIODevice* output() const;

  void setOutputSchema(QIODevice* output);

  /**
   * @brief Returns \code true if the XSL output will be formatted.
   * @return \code true if the ouptut will be formatted, \code false otherwise.
   */
  bool formatXmlOutput() const;

  /**
   * @brief Formats the XSL output to be human-readable.
   *
   * It is assumed that the XSL output is valid XML. The output will be
   * formatted with an indentation depth of four spaces. Note that setting
   * \e format to \code true increases compuational overhead and memory
   * requirements and is usually only done for testing or debugging purposes.
   */
  void setFormatXmlOutput(bool format);

  /**
   * @brief Transforms an XML input via a XSL transformation.
   *
   * This method assumes that the input set via setInput() or supplied
   * in the constructor is a valid, non empty XML fragment.
   *
   * @return
   */
  bool transform();

  /**
   * @brief Sets the XSL transformation.
   *
   * Use this method to set the XSL transformation for this instance. Trying
   * to transform the input without setting a transformation will result in
   * runtime errors.
   *
   * @param The XSL transformation.
   */
  void setXslTransformation(QIODevice* transformation);

  /**
   * @brief XSL to be injected in the main XSL.
   *
   * This can be used to potentially overwrite templates.
   * To avoid ambiguity, specify a priority > 1 in your overwriting templates
   *
   * \param transformation Extra XSL transformation fragment.
   */
  void setXslExtraTransformation(QIODevice* transformation);
  void setXslExtraTransformations(const QList<QIODevice*>& transformations);

  /**
   *  @brief Binds the variable name to the value so that $name can be used
   *  from within the query to refer to the value.
   *  In the default XslTransformation, the widget classes are variable and can
   *  be replaced with a new binding.
   *  @sa QXmlQuery::bindVariable()
   */
  void bindVariable(const QString& name, const QVariant& value);

  void setValidateOutput(bool validate);

  /** @brief returns true if an error occured
   *  transform() sets the error flag if an error occured when transforming the
   *  XML file into XSL.
   *  \sa errorString
   */
  virtual bool error() const;

  /** @brief Error message if any
   *  transform() sets the error message if an error occured when transforming
   * the XML file into XSL.
   *  \sa error
   */
  virtual QString errorString() const;

private:

  QScopedPointer<ctkCmdLineModuleXslTransformPrivate> d;

};


#endif // CTKCMDLINEMODULEXSLTRANSFORM_H
