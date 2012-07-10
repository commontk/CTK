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

#ifndef CTKCMDLINEMODULEDESCRIPTIONPRIVATE_H
#define CTKCMDLINEMODULEDESCRIPTIONPRIVATE_H

#include <QString>
#include <QIcon>

class ctkCmdLineModuleParameterGroup;

struct ctkCmdLineModuleDescriptionPrivate : public QSharedData
{
  QString Title;
  QString Category;
  QString Description;
  QString Version;
  QString DocumentationURL;
  QString License;
  QString Acknowledgements;
  QString Contributor;
  QString Type;
  QString Target;
  QString Location;
  QString AlternativeType;
  QString AlternativeTarget;
  QString AlternativeLocation;

  QIcon Logo;

  QList<ctkCmdLineModuleParameterGroup> ParameterGroups;

};

#endif // CTKCMDLINEMODULEDESCRIPTIONPRIVATE_H
