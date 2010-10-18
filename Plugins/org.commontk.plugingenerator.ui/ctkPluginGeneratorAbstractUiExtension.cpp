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

#include "ctkPluginGeneratorAbstractUiExtension.h"

class ctkPluginGeneratorAbstractUiExtensionPrivate
{
public:

  ctkPluginGeneratorAbstractUiExtensionPrivate()
    : extensionWidget(0)
  {

  }

  QWidget* extensionWidget;

  QString description;
  QString title;
  QString message;
  QIcon icon;
};

ctkPluginGeneratorAbstractUiExtension::ctkPluginGeneratorAbstractUiExtension()
  : d_ptr(new ctkPluginGeneratorAbstractUiExtensionPrivate())
{

}

ctkPluginGeneratorAbstractUiExtension::~ctkPluginGeneratorAbstractUiExtension()
{

}

QWidget* ctkPluginGeneratorAbstractUiExtension::getWidget()
{
  Q_D(ctkPluginGeneratorAbstractUiExtension);
  if (d->extensionWidget == 0)
  {
    d->extensionWidget = this->createWidget();
  }

  return d->extensionWidget;
}

QString ctkPluginGeneratorAbstractUiExtension::getDescription() const
{
  Q_D(const ctkPluginGeneratorAbstractUiExtension);
  return d->description;
}

QString ctkPluginGeneratorAbstractUiExtension::getTitle() const
{
  Q_D(const ctkPluginGeneratorAbstractUiExtension);
  return d->title;
}

QString ctkPluginGeneratorAbstractUiExtension::getMessage() const
{
  Q_D(const ctkPluginGeneratorAbstractUiExtension);
  return d->message;
}

QIcon ctkPluginGeneratorAbstractUiExtension::getIcon() const
{
  Q_D(const ctkPluginGeneratorAbstractUiExtension);
  return d->icon;
}

void ctkPluginGeneratorAbstractUiExtension::setDescription(const QString& description)
{
  Q_D(ctkPluginGeneratorAbstractUiExtension);
  if (d->description != description)
  {
    d->description = description;
    emit descriptionChanged(description);
  }
}

void ctkPluginGeneratorAbstractUiExtension::setTitle(const QString& title)
{
  Q_D(ctkPluginGeneratorAbstractUiExtension);
  if (d->title != title)
  {
    d->title = title;
    emit titleChanged(title);
  }
}

void ctkPluginGeneratorAbstractUiExtension::setMessage(const QString& msg)
{
  Q_D(ctkPluginGeneratorAbstractUiExtension);
  if (d->message != msg)
  {
    d->message = msg;
    emit messageChanged(msg);
  }
}

void ctkPluginGeneratorAbstractUiExtension::setIcon(const QIcon& icon)
{
  Q_D(ctkPluginGeneratorAbstractUiExtension);
  d->icon = icon;
  emit iconChanged(icon);
}

