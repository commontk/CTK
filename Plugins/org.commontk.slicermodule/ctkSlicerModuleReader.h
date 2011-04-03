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

#ifndef __ctkSlicerModuleReader_h
#define __ctkSlicerModuleReader_h

// Qt includes
#include <QRegExp>
#include <QStack>
#include <QXmlAttributes>
#include <QXmlDefaultHandler>

// CTK includes
#include "ctkModuleDescriptionReader.h"

/**
 * Reader of Slicer Module XML description
 * Freely inspired from 
 * Slicer/Libs/SlicerExecutionModel/ModuleDescriptionParser/ModuleDescriptionParser.cxx
 */
class ctkSlicerModuleReader : public ctkModuleDescriptionReader
{
  Q_OBJECT
public:
  virtual void update();
  bool validate()const; 
};

/**
 * Utility class to parse the Slicer Module XML descriptions using the
 * SAX interface.
 */
class ctkSlicerModuleHandler: public QXmlDefaultHandler
{
public:
  ctkSlicerModuleHandler();
  bool characters(const QString& ch);
  virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
  virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts );
  
  void setModuleDescription(ctkModuleDescription* moduleDescription);
  ctkModuleDescription* moduleDescription()const;

protected:

  bool startExecutableElement(const QString& namespaceURI, const QString& localName,
                              const QString& name, const QXmlAttributes& atts);

  bool startGroupElement(const QString& namespaceURI, const QString& localName,
                         const QString& name, const QXmlAttributes& atts);

  bool startParameterElement(const QString& namespaceURI, const QString& localName,
                             const QString& name, const QXmlAttributes& atts);

  bool endExecutableElement(const QString& namespaceURI, const QString& localName,
                            const QString& name);

  bool endGroupElement(const QString& namespaceURI, const QString& localName,
                       const QString& name);

  bool endParameterElement(const QString& namespaceURI, const QString& localName,
                           const QString& name);
                            
  ctkModuleDescription* ModuleDescription;
  struct ParserState{
    ctkModuleParameter*      CurrentParameter;
    ctkModuleParameterGroup* CurrentGroup;
    QString                  CurrentText;
    int                      InExecutable;
    int                      InGroup;
    int                      InParameter;
  };
  ParserState State;
  QRegExp     ParamValidator;
};

#endif
