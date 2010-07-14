/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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

#include "ctkPluginGeneratorAbstractUiSection.h"

ctkPluginGeneratorAbstractUiSection::ctkPluginGeneratorAbstractUiSection()
  : sectionWidget(0)
{

}

ctkPluginGeneratorAbstractUiSection::~ctkPluginGeneratorAbstractUiSection()
{

}

QWidget* ctkPluginGeneratorAbstractUiSection::getWidget()
{
  return sectionWidget;
}

QString ctkPluginGeneratorAbstractUiSection::getDescription() const
{
  return this->description;
}

QString ctkPluginGeneratorAbstractUiSection::getTitle() const
{
  return this->title;
}

QString ctkPluginGeneratorAbstractUiSection::getErrorMessage() const
{
  return this->errorMessage;
}

QString ctkPluginGeneratorAbstractUiSection::getMessage() const
{
  return this->message;
}

QIcon ctkPluginGeneratorAbstractUiSection::getIcon() const
{
  return this->icon;
}

void ctkPluginGeneratorAbstractUiSection::setDescription(const QString& description)
{
  if (this->description != description)
  {
    this->description = description;
    emit descriptionChanged(description);
  }
}

void ctkPluginGeneratorAbstractUiSection::setTitle(const QString& title)
{
  if (this->title != title)
  {
    this->title = title;
    emit titleChanged(title);
  }
}

void ctkPluginGeneratorAbstractUiSection::setErrorMessage(const QString& errorMsg)
{
  if (this->errorMessage != errorMsg)
  {
    this->errorMessage = errorMsg;
    emit errorMessageChanged(errorMsg);
  }
}

void ctkPluginGeneratorAbstractUiSection::setMessage(const QString& msg)
{
  if (this->message != msg)
  {
    this->message = msg;
    emit messageChanged(msg);
  }
}

void ctkPluginGeneratorAbstractUiSection::setIcon(const QIcon& icon)
{
  this->icon = icon;
  emit iconChanged(icon);
}

