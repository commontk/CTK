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

#include "ctkSlicerModuleReader.h"
#include <QtXml/QDomDocument>

// ----------------------------------------------------------------------------
void ctkSlicerModuleReader::update()
{
  QDomDocument domDocument;
  QString errorStr;
  int errorLine;
  int errorColumn;

  if (!domDocument.setContent(XmlContent, &errorStr, &errorLine,
    &errorColumn)) {
      throw std::runtime_error( 
        tr("Parse error at line %1, column %2:\n%3")
        .arg(errorLine)
        .arg(errorColumn)
        .arg(errorStr).toStdString().c_str() );
      return ;
  }

}
