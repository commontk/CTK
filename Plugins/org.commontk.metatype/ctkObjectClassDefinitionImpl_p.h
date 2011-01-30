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


#ifndef CTKOBJECTCLASSDEFINITIONIMPL_P_H
#define CTKOBJECTCLASSDEFINITIONIMPL_P_H

#include <service/metatype/ctkObjectClassDefinition.h>
#include <ctkPluginLocalization.h>

#include "ctkMTIcon_p.h"
#include "ctkMTLocalizationElement_p.h"

class ctkAttributeDefinitionImpl;

/**
 * Implementation of ObjectClassDefinition
 */
class ctkObjectClassDefinitionImpl : public ctkObjectClassDefinition
{

private:

  static const QChar LOCALE_SEP; // = '_'

  QString _name;
  QString _id;
  QString _description;

  ctkMTLocalizationElement _locElem;

  int _type;
  QList<QSharedPointer<ctkAttributeDefinitionImpl> > _required;
  QList<QSharedPointer<ctkAttributeDefinitionImpl> > _optional;
  ctkMTIcon _icon;

public:

  static const int PID; // = 0
  static const int FPID; // = 1

  /*
   * Constructor of class ctkObjectClassDefinitionImpl.
   */
  ctkObjectClassDefinitionImpl(const QString& name, const QString& description,
                               const QString& id, const QString& localization,
                               const QString& context, int type = 0);

  ctkObjectClassDefinitionImpl(const ctkObjectClassDefinitionImpl& other);

  /*
   * @see ctkObjectClassDefinition#getName()
   */
  QString getName() const;

  /**
   * Method to set the name of ctkObjectClassDefinition.
  */
  void setName(const QString& name);

  /*
   * @see ctkObjectClassDefinition#getID()
   */
  QString getID() const;

  /*
   * @see ctkObjectClassDefinition#getDescription()
   */
  QString getDescription() const;

  /*
   * Method to set the description of ctkObjectClassDefinition.
  */
  void setDescription(const QString& description);

 /*
  * @see ctkObjectClassDefinition#getAttributeDefinitions(Filter)
  */
  QList<ctkAttributeDefinitionPtr> getAttributeDefinitions(Filter filter);

  /*
   * Method to add one new AD to ctkObjectClassDefinition.
   */
  void addAttributeDefinition(const QSharedPointer<ctkAttributeDefinitionImpl>& ad,
                              bool isRequired);

  /*
   * @see ctkObjectClassDefinition#getIcon(int)
   */
  QByteArray getIcon(int sizeHint) const;

  /**
   * Method to set the icon of ctkObjectClassDefinition.
  */
  void setIcon(const ctkMTIcon& icon);

  /**
   * Method to get the type of ctkObjectClassDefinition.
  */
  int getType() const;

  /**
   * Method to set the type of ctkObjectClassDefinition.
   */
  void setType(int type);

  /**
   * Method to set the plugin localization object for this OCD and all its ADs.
   */
  void setPluginLocalization(const QLocale& assignedLocale, const QSharedPointer<ctkPlugin>& plugin);

  QString getLocalization() const;

};

typedef QSharedPointer<ctkObjectClassDefinitionImpl> ctkObjectClassDefinitionImplPtr;

#endif // CTKOBJECTCLASSDEFINITIONIMPL_P_H
