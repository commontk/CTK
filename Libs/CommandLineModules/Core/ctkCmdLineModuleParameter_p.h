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

#ifndef CTKCMDLINEMODULEPARAMETERPRIVATE_H
#define CTKCMDLINEMODULEPARAMETERPRIVATE_H

#include "ctkCmdLineModuleParameter.h"

#include <QString>
#include <QStringList>

struct ctkCmdLineModuleParameterPrivate : public QSharedData
{
  ctkCmdLineModuleParameterPrivate();

  QStringList splitAndTrim(const QString& str, const QString& separator);

  void setFileExtensionsAsString(const QString& extensions);

  QString Tag;
  QString Name;
  QString Description;
  QString Label;
  QString Type;
  bool Hidden;
  QString Default;
  QString Flag;
  QString LongFlag;
  bool Constraints;
  QString Minimum;
  QString Maximum;
  QString Step;
  QString Channel;
  int Index;
  int Multiple;
  QString FileExtensionsAsString;
  QStringList FileExtensions;
  QString CoordinateSystem;
  QStringList Elements;
  QString FlagAliasesAsString;
  QString DeprecatedFlagAliasesAsString;
  QString LongFlagAliasesAsString;
  QString DeprecatedLongFlagAliasesAsString;
  QStringList FlagAliases;
  QStringList DeprecatedFlagAliases;
  QStringList LongFlagAliases;
  QStringList DeprecatedLongFlagAliases;

};

#endif // CTKCMDLINEMODULEPARAMETERPRIVATE_H
