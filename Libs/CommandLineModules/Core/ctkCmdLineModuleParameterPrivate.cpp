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

#include "ctkCmdLineModuleParameterPrivate.h"

ctkCmdLineModuleParameterPrivate::ctkCmdLineModuleParameterPrivate()
  : Hidden(false), Constraints(false), Index(-1), Multiple(false), Aggregate("false")
{}

QStringList ctkCmdLineModuleParameterPrivate::splitAndTrim(const QString& str, const QString& separator)
{
  QStringList l = str.split(separator, QString::SkipEmptyParts);
  l.removeDuplicates();
  // trim the strings
  QMutableStringListIterator i(l);
  while(i.hasNext())
  {
    QString& n = i.next();
    n = n.trimmed();
  }
  return l;
}

void ctkCmdLineModuleParameterPrivate::setFileExtensionsAsString(const QString& extensions)
{
  FileExtensions = splitAndTrim(extensions, ",");
  FileExtensionsAsString = FileExtensions.join(",");
}
