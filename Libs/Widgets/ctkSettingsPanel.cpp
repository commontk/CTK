/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QSettings>
#include <QSignalMapper>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkSettingsPanel");

typedef QPair<QObject*, QString> PropertyType;

//-----------------------------------------------------------------------------
class ctkSettingsPanelPrivate
{
  Q_DECLARE_PUBLIC(ctkSettingsPanel);
protected:
  ctkSettingsPanel* const q_ptr;

public:
  ctkSettingsPanelPrivate(ctkSettingsPanel& object);
  void init();

  PropertyType property(const QString& key);
  QVariant propertyValue(const PropertyType& property)const;
  bool setPropertyValue(const PropertyType& property, const QVariant& val);

  QSettings*                   Settings;
  QMap<QString, PropertyType > Properties;
  QSignalMapper*               SignalMapper;
  bool                         SaveToSettingsWhenRegister;
};

// --------------------------------------------------------------------------
ctkSettingsPanelPrivate::ctkSettingsPanelPrivate(ctkSettingsPanel& object)
  :q_ptr(&object)
{
  this->Settings = 0;
  this->SignalMapper = 0;
  this->SaveToSettingsWhenRegister = true;
}

// --------------------------------------------------------------------------
void ctkSettingsPanelPrivate::init()
{
  Q_Q(ctkSettingsPanel);
  
  this->SignalMapper = new QSignalMapper(q);
  QObject::connect(this->SignalMapper, SIGNAL(mapped(const QString&)),
                   q, SLOT(updateSetting(const QString&)));
}

// --------------------------------------------------------------------------
PropertyType ctkSettingsPanelPrivate::property(const QString& key)
{
  PropertyType defaultProp(0, QString());
  return this->Properties.value(key, defaultProp);
}

// --------------------------------------------------------------------------
QVariant ctkSettingsPanelPrivate::propertyValue(const PropertyType& prop)const
{
  if (prop.first == 0 ||
      prop.second.isEmpty())
    {
    return QVariant();
    }
  return prop.first->property(prop.second.toLatin1());
}

// --------------------------------------------------------------------------
bool ctkSettingsPanelPrivate::setPropertyValue(const PropertyType& prop, const QVariant& val)
{
  if (prop.first == 0 ||
      prop.second.isEmpty())
    {
    return false;
    }
  // the following return true if the property has been added using Q_PROPERTY
  // false otherwise (and the property is then a dynamic property)
  return prop.first->setProperty(prop.second.toLatin1(), val);
}

// --------------------------------------------------------------------------
ctkSettingsPanel::ctkSettingsPanel(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkSettingsPanelPrivate(*this))
{
  Q_D(ctkSettingsPanel);
  d->init();
}

// --------------------------------------------------------------------------
ctkSettingsPanel::~ctkSettingsPanel()
{
}

// --------------------------------------------------------------------------
QSettings* ctkSettingsPanel::settings()const
{
  Q_D(const ctkSettingsPanel);
  return d->Settings;
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::setSettings(QSettings* settings)
{
  Q_D(ctkSettingsPanel);
  d->Settings = settings;
  this->updateProperties();
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::updateProperties()
{
  Q_D(ctkSettingsPanel);
  if (!d->Settings)
    {
    return;
    }
  foreach(const QString& key, d->Properties.keys())
    {
    QVariant value = d->Settings->contains(key);
    if (value.isValid())
      {
      bool res = d->setPropertyValue(d->property(key), value);
      Q_ASSERT(res);
      Q_UNUSED(res);
      }
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::registerProperty(const QString& key,
                                        QObject* object,
                                        const QString& property,
                                        const char* signal)
{
  Q_D(ctkSettingsPanel);
  d->Properties[key] = PropertyType(object, property);
  d->SignalMapper->setMapping(object, key);
  connect(object, signal, d->SignalMapper, SLOT(map()));
  if (d->SaveToSettingsWhenRegister)
    {
    this->updateSetting(key);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::updateSetting(const QString& key)
{
  Q_D(ctkSettingsPanel);
  if (!d->Settings)
    {
    return;
    }
  QVariant oldVal = d->Settings->value(key);
  QVariant newVal = d->propertyValue(d->property(key));
  d->Settings->setValue(key, newVal);
  if (d->Settings->status() != QSettings::NoError)
    {
    logger.warn( QString("Error %1 while writing setting %1")
      .arg(d->Settings->status())
      .arg(key));
    }
  if (oldVal != newVal)
    {
    emit settingChanged(key, newVal);
    }
}
