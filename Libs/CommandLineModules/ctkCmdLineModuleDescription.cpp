/*=============================================================================

Library: CTK

Copyright (c) 2010 Brigham and Women's Hospital (BWH) All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use d file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/

#include "ctkCmdLineModuleDescription.h"
#include <iostream>
#include "QFile"
#include "QTextStream"

struct ctkCmdLineModuleDescriptionPrivate
{
  ~ctkCmdLineModuleDescriptionPrivate()
  {
    qDeleteAll(ParameterGroups);
  }

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

  QList<ctkCmdLineModuleParameterGroup*> ParameterGroups;

  //ModuleProcessInformation ProcessInformation;
};

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription::ctkCmdLineModuleDescription()
  : d_ptr(new ctkCmdLineModuleDescriptionPrivate)
{
}

//----------------------------------------------------------------------------
ctkCmdLineModuleDescription::~ctkCmdLineModuleDescription()
{
  delete d_ptr;
}


//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setCategory(const QString& cat)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Category = cat;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::category() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Category;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setTitle(const QString& title)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Title = title;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::title() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Title;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setDescription(const QString& description)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::description() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setVersion(const QString& version)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Version = version;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::version() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Version;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setDocumentationURL(const QString& documentationURL)
{
  Q_D(ctkCmdLineModuleDescription);
  d->DocumentationURL = documentationURL;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::documentationURL() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->DocumentationURL;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setLicense(const QString& license)
{
  Q_D(ctkCmdLineModuleDescription);
  d->License = license;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::license() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->License;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setAcknowledgements(const QString& acknowledgements)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Acknowledgements = acknowledgements;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::acknowledgements() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Acknowledgements;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setContributor(const QString& contributor)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Contributor = contributor;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::contributor() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Contributor;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setLocation(const QString& target)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Location = target;
}

//----------------------------------------------------------------------------
QString ctkCmdLineModuleDescription::location() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Location;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setLogo(const QIcon& logo)
{
  Q_D(ctkCmdLineModuleDescription);
  d->Logo = logo;
}

//----------------------------------------------------------------------------
QIcon ctkCmdLineModuleDescription::logo() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->Logo;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::addParameterGroup(ctkCmdLineModuleParameterGroup* group)
{
  Q_D(ctkCmdLineModuleDescription);
  d->ParameterGroups.push_back(group);
}

//----------------------------------------------------------------------------
QList<ctkCmdLineModuleParameterGroup*> ctkCmdLineModuleDescription::parameterGroups() const
{
  Q_D(const ctkCmdLineModuleDescription);
  return d->ParameterGroups;
}

//----------------------------------------------------------------------------
void ctkCmdLineModuleDescription::setParameterGroups(const QList<ctkCmdLineModuleParameterGroup*>& groups)
{
  Q_D(ctkCmdLineModuleDescription);
  d->ParameterGroups = groups;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::hasParameter(const QString& name) const
{
  Q_D(const ctkCmdLineModuleDescription);
  // iterate over each parameter group
  foreach(const ctkCmdLineModuleParameterGroup* group, d->ParameterGroups)
  {
    if (group->hasParameter(name)) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkCmdLineModuleParameter* ctkCmdLineModuleDescription::parameter(const QString& name) const
{
  Q_D(const ctkCmdLineModuleDescription);
  foreach(const ctkCmdLineModuleParameterGroup* group, d->ParameterGroups)
  {
    ctkCmdLineModuleParameter* param = group->parameter(name);
    if (param) return param;
  }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::hasReturnParameters() const
{
  Q_D(const ctkCmdLineModuleDescription);
  // iterate over each parameter group
  foreach(const ctkCmdLineModuleParameterGroup* group, d->ParameterGroups)
  {
    if (group->hasReturnParameters()) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::setParameterDefaultValue(const QString& name,
                                                    const QString& value)
{
  ctkCmdLineModuleParameter* param = parameter(name);
  if (param)
  {
    param->setDefaultValue(value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription ::readParameterFile(const QString& filename)
{
  bool modified = false;

  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    std::cout << "Parameter file " << filename.toStdString( ) << " could not be opened." << '\n';
    return false;
  }

  QTextStream in(&file);
  while (!in.atEnd())
  {
    QString line = in.readLine();

    // split the line into key: value
    QString key, value;

    line = line.trimmed();
    QStringList list = line.split( "=" );
    key = list[ 0 ].trimmed();
    if ( list.size() == 1 )
    {
      continue;
    }
    value = list[ 1 ].trimmed();

    
    // std::cout << "key=" << key << ", value=" << value << "!" << endl;

    ctkCmdLineModuleParameter* param = this->parameter(key);
    if (param)
    {
      if (value != param->defaultValue())
      {
        param->setDefaultValue(value);
        modified = true;

        // multiple="true" may have to be handled differently
      }
    }
  }

  return modified;
}

//----------------------------------------------------------------------------
bool ctkCmdLineModuleDescription::
writeParameterFile(const QString& filename, bool withHandlesToBulkParameters)const
{
  Q_D(const ctkCmdLineModuleDescription);

  QFile rtp(filename);

  if (!rtp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString() << " could not be opened for writing." << '\n';
    return false;
    }

  QTextStream in(&rtp);
  // iterate over each parameter group
  foreach(const ctkCmdLineModuleParameterGroup* group, d->ParameterGroups)
  {
    group->writeParameterFile(in, withHandlesToBulkParameters);
  }

  return true;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkCmdLineModuleDescription &module)
{
  os << "Title: " << module.title() << '\n';
  os << "Category: " << module.category() << '\n';
  os << "Description: " << module.description() << '\n';
  os << "Version: " << module.version() << '\n';
  os << "DocumentationURL: " << module.documentationURL() << '\n';
  os << "License: " << module.license() << '\n';
  os << "Contributor: " << module.contributor() << '\n';
  os << "Acknowledgements: " << module.acknowledgements() << '\n';
  os << "Location: " << module.location() << '\n';
  //os << "Logo: " << module.GetLogo() << '\n';

  //os << "ProcessInformation: " << '\n'
  //   << *(module.GetProcessInformation());

  os << "ParameterGroups: " << '\n';
  foreach(const ctkCmdLineModuleParameterGroup* group, module.parameterGroups())
  {
    os << *group;
  }
  return os;
}

