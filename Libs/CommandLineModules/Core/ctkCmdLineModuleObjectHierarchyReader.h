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

#ifndef CTKCMDLINEMODULEOBJECTHIERARCHYREADER_H
#define CTKCMDLINEMODULEOBJECTHIERARCHYREADER_H

#include <QString>
#include <QScopedPointer>
#include <QVariant>

#include <ctkCommandLineModulesCoreExport.h>

class QObject;

class ctkCmdLineModuleObjectHierarchyReaderPrivate;

class CTK_CMDLINEMODULECORE_EXPORT ctkCmdLineModuleObjectHierarchyReader
{

public:

  enum TokenType {
    NoToken,
    Executable,
    ParameterGroup,
    Parameter
  };

  ctkCmdLineModuleObjectHierarchyReader(QObject* root = 0);
  ~ctkCmdLineModuleObjectHierarchyReader();

  void setRootObject(QObject* root);
  void clear();

  bool atEnd() const;

  bool isParameterGroup() const;
  bool isParameter() const;

  QString name() const;
  QString label() const;
  QVariant value() const;

  void setValue(const QVariant& value);

  QString flag() const;
  QString longFlag() const;
  int index() const;

  bool isMultiple() const;


  QVariant property(const QString& propName) const;

  TokenType readNext() const;
  bool readNextExecutable() const;
  bool readNextParameterGroup() const;
  bool readNextParameter() const;

  TokenType tokenType() const;

private:

  QScopedPointer<ctkCmdLineModuleObjectHierarchyReaderPrivate> d;

};

#endif // CTKCMDLINEMODULEOBJECTHIERARCHYREADER_H
