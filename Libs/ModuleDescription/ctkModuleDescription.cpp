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

#include "ctkModuleDescription.h"
#include <iostream>
#include "QFile"
#include "QTextStream"

struct ctkModuleDescriptionPrivate
{
  ~ctkModuleDescriptionPrivate()
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

  QList<ctkModuleParameterGroup*> ParameterGroups;

  //ModuleProcessInformation ProcessInformation;
};

//----------------------------------------------------------------------------
ctkModuleDescription::ctkModuleDescription()
  : d_ptr(new ctkModuleDescriptionPrivate)
{
}

//----------------------------------------------------------------------------
ctkModuleDescription::~ctkModuleDescription()
{
  delete d_ptr;
}


//----------------------------------------------------------------------------
void ctkModuleDescription::setCategory(const QString& cat)
{
  Q_D(ctkModuleDescription);
  d->Category = cat;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::category() const
{
  Q_D(const ctkModuleDescription);
  return d->Category;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setTitle(const QString& title)
{
  Q_D(ctkModuleDescription);
  d->Title = title;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::title() const
{
  Q_D(const ctkModuleDescription);
  return d->Title;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setDescription(const QString& description)
{
  Q_D(ctkModuleDescription);
  d->Description = description;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::description() const
{
  Q_D(const ctkModuleDescription);
  return d->Description;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setVersion(const QString& version)
{
  Q_D(ctkModuleDescription);
  d->Version = version;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::version() const
{
  Q_D(const ctkModuleDescription);
  return d->Version;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setDocumentationURL(const QString& documentationURL)
{
  Q_D(ctkModuleDescription);
  d->DocumentationURL = documentationURL;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::documentationURL() const
{
  Q_D(const ctkModuleDescription);
  return d->DocumentationURL;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setLicense(const QString& license)
{
  Q_D(ctkModuleDescription);
  d->License = license;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::license() const
{
  Q_D(const ctkModuleDescription);
  return d->License;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setAcknowledgements(const QString& acknowledgements)
{
  Q_D(ctkModuleDescription);
  d->Acknowledgements = acknowledgements;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::acknowledgements() const
{
  Q_D(const ctkModuleDescription);
  return d->Acknowledgements;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setContributor(const QString& contributor)
{
  Q_D(ctkModuleDescription);
  d->Contributor = contributor;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::contributor() const
{
  Q_D(const ctkModuleDescription);
  return d->Contributor;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setLocation(const QString& target)
{
  Q_D(ctkModuleDescription);
  d->Location = target;
}

//----------------------------------------------------------------------------
QString ctkModuleDescription::location() const
{
  Q_D(const ctkModuleDescription);
  return d->Location;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setLogo(const QIcon& logo)
{
  Q_D(ctkModuleDescription);
  d->Logo = logo;
}

//----------------------------------------------------------------------------
QIcon ctkModuleDescription::logo() const
{
  Q_D(const ctkModuleDescription);
  return d->Logo;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::addParameterGroup(ctkModuleParameterGroup* group)
{
  Q_D(ctkModuleDescription);
  d->ParameterGroups.push_back(group);
}

//----------------------------------------------------------------------------
QList<ctkModuleParameterGroup*> ctkModuleDescription::parameterGroups() const
{
  Q_D(const ctkModuleDescription);
  return d->ParameterGroups;
}

//----------------------------------------------------------------------------
void ctkModuleDescription::setParameterGroups(const QList<ctkModuleParameterGroup*>& groups)
{
  Q_D(ctkModuleDescription);
  d->ParameterGroups = groups;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::hasParameter(const QString& name) const
{
  Q_D(const ctkModuleDescription);
  // iterate over each parameter group
  foreach(const ctkModuleParameterGroup* group, d->ParameterGroups)
  {
    if (group->hasParameter(name)) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
ctkModuleParameter* ctkModuleDescription::parameter(const QString& name) const
{
  Q_D(const ctkModuleDescription);
  foreach(const ctkModuleParameterGroup* group, d->ParameterGroups)
  {
    ctkModuleParameter* param = group->parameter(name);
    if (param) return param;
  }
  return 0;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::hasReturnParameters() const
{
  Q_D(const ctkModuleDescription);
  // iterate over each parameter group
  foreach(const ctkModuleParameterGroup* group, d->ParameterGroups)
  {
    if (group->hasReturnParameters()) return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription::setParameterDefaultValue(const QString& name,
                                                    const QString& value)
{
  ctkModuleParameter* param = parameter(name);
  if (param)
  {
    param->setDefaultValue(value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool ctkModuleDescription ::readParameterFile(const QString& filename)
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

    ctkModuleParameter* param = this->parameter(key);
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
bool ctkModuleDescription::
writeParameterFile(const QString& filename, bool withHandlesToBulkParameters)const
{
  Q_D(const ctkModuleDescription);

  QFile rtp(filename);

  if (!rtp.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    std::cout << "Parameter file " << filename.toStdString() << " could not be opened for writing." << '\n';
    return false;
    }

  QTextStream in(&rtp);
  // iterate over each parameter group
  foreach(const ctkModuleParameterGroup* group, d->ParameterGroups)
  {
    group->writeParameterFile(in, withHandlesToBulkParameters);
  }

  return true;
}

//----------------------------------------------------------------------------
QTextStream & operator<<(QTextStream &os, const ctkModuleDescription &module)
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
  foreach(const ctkModuleParameterGroup* group, module.parameterGroups())
  {
    os << *group;
  }
  return os;
}

