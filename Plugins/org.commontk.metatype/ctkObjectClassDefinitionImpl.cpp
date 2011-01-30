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


#include "ctkObjectClassDefinitionImpl_p.h"

#include "ctkAttributeDefinitionImpl_p.h"

#include <ctkPlugin.h>
#include <ctkPluginConstants.h>

const int ctkObjectClassDefinitionImpl::PID = 0;
const int ctkObjectClassDefinitionImpl::FPID = 1;
const QChar ctkObjectClassDefinitionImpl::LOCALE_SEP = '_';

ctkObjectClassDefinitionImpl::ctkObjectClassDefinitionImpl(const QString& name, const QString& description,
                                                           const QString& id, const QString& localization,
                                                           const QString& context, int type)
  : _name(name), _id(id), _description(description), _locElem(localization, context), _type(type)
{
}

ctkObjectClassDefinitionImpl::ctkObjectClassDefinitionImpl(const ctkObjectClassDefinitionImpl& other)
  : _name(other._name), _id(other._id), _description(other._description),
    _locElem(other._locElem), _type(other._type), _icon(other._icon)
{
  for (int i = 0; i < _required.size(); i++)
  {
    ctkAttributeDefinitionImplPtr ad(new ctkAttributeDefinitionImpl(*_required.value(i).data()));
    this->addAttributeDefinition(ad, true);
  }
  for (int i = 0; i < _optional.size(); i++)
  {
    ctkAttributeDefinitionImplPtr ad(new ctkAttributeDefinitionImpl(*_optional.value(i).data()));
    this->addAttributeDefinition(ad, false);
  }
}

QString ctkObjectClassDefinitionImpl::getName() const
{
  return _locElem.getLocalized(_name);
}

void ctkObjectClassDefinitionImpl::setName(const QString& name)
{
  _name = name;
}

QString ctkObjectClassDefinitionImpl::getID() const
{
  return _id;
}

QString ctkObjectClassDefinitionImpl::getDescription() const
{
  return _locElem.getLocalized(_description);
}

void ctkObjectClassDefinitionImpl::setDescription(const QString& description)
{
  _description = description;
}

QList<ctkAttributeDefinitionPtr> ctkObjectClassDefinitionImpl::getAttributeDefinitions(Filter filter)
{
  QList<ctkAttributeDefinitionPtr> atts;
  switch (filter)
  {
  case REQUIRED:
  {
    foreach(ctkAttributeDefinitionImplPtr impl, _required)
    {
      atts.push_back(impl);
    }
    return atts;
  }
  case OPTIONAL:
  {
    foreach(ctkAttributeDefinitionImplPtr impl, _optional)
    {
      atts.push_back(impl);
    }
    return atts;
  }
  case ALL:
  default :
  {
    foreach(ctkAttributeDefinitionImplPtr impl, _required)
    {
      atts.push_back(impl);
    }
    foreach(ctkAttributeDefinitionImplPtr impl, _optional)
    {
      atts.push_back(impl);
    }
    return atts;
  }
  }
}

void ctkObjectClassDefinitionImpl::addAttributeDefinition(const ctkAttributeDefinitionImplPtr& ad, bool isRequired)
{
  if (isRequired)
  {
    _required.push_back(ad);
  }
  else
  {
    _optional.push_back(ad);
  }
}

QByteArray ctkObjectClassDefinitionImpl::getIcon(int sizeHint) const
{
  if (!_icon || _icon.getIconSize() != sizeHint)
  {
    return QByteArray();
  }
  QSharedPointer<ctkPlugin> p = _icon.getIconPlugin();
  return p->getResource(_locElem.getLocalized(_icon.getIconName()));
}

void ctkObjectClassDefinitionImpl::setIcon(const ctkMTIcon& icon)
{
  _icon = icon;
}

int ctkObjectClassDefinitionImpl::getType() const
{
  return _type;
}

void ctkObjectClassDefinitionImpl::setType(int type)
{
  _type = type;
}

void ctkObjectClassDefinitionImpl::setPluginLocalization(const QLocale& assignedLocale, const QSharedPointer<ctkPlugin>& plugin)
{
  ctkPluginLocalization pluginLoc = plugin->getPluginLocalization(assignedLocale, _locElem.getLocalizationBase());
  _locElem.setPluginLocalization(pluginLoc);

  foreach(ctkAttributeDefinitionImplPtr impl, _required)
  {
    impl->setPluginLocalization(pluginLoc);
  }

  foreach(ctkAttributeDefinitionImplPtr impl, _optional)
  {
    impl->setPluginLocalization(pluginLoc);
  }
}

QString ctkObjectClassDefinitionImpl::getLocalization() const
{
  return _locElem.getLocalizationBase();
}

