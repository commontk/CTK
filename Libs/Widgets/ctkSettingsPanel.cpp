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

struct PropertyType
{
  PropertyType();
  QObject* Object;
  QString  Property;
  QVariant PreviousValue;
  QVariant DefaultValue;

  QVariant value()const;
  bool setValue(const QVariant& value);
};

// --------------------------------------------------------------------------
PropertyType::PropertyType()
  : Object(0)
{
}

// --------------------------------------------------------------------------
QVariant PropertyType::value()const
{
  if (this->Object == 0 ||
      this->Property.isEmpty())
    {
    return QVariant();
    }
  return this->Object->property(this->Property.toLatin1());
}

// --------------------------------------------------------------------------
bool PropertyType::setValue(const QVariant& val)
{
  if (this->Object == 0 || this->Property.isEmpty())
    {
    Q_ASSERT(this->Object && !this->Property.isEmpty());
    return false;
    }
  // the following returns true if the property has been added using Q_PROPERTY
  // false otherwise (and the property is then a dynamic property)
  return this->Object->setProperty(this->Property.toLatin1(), val);
}
//-----------------------------------------------------------------------------
class ctkSettingsPanelPrivate
{
  Q_DECLARE_PUBLIC(ctkSettingsPanel);
protected:
  ctkSettingsPanel* const q_ptr;

public:
  ctkSettingsPanelPrivate(ctkSettingsPanel& object);
  void init();

  QSettings*                  Settings;
  QMap<QString, PropertyType> Properties;
  QSignalMapper*              SignalMapper;
  bool                        SaveToSettingsWhenRegister;
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
  this->applySettings();
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
  if (d->Settings == settings)
    {
    return;
    }
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
    if (d->Settings->contains(key))
      {
      QVariant value = d->Settings->value(key);
      PropertyType& prop = d->Properties[key];
      bool res = prop.setValue(value);
      Q_ASSERT(res);
      Q_UNUSED(res);
      prop.PreviousValue = value;
      }
    else
      {
      this->updateSetting(key);
      }
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
  this->setSetting(key, d->Properties[key].value());
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::setSetting(const QString& key, const QVariant& newVal)
{
  Q_D(ctkSettingsPanel);
  QVariant oldVal = d->Settings->value(key);
  d->Settings->setValue(key, newVal);
  d->Properties[key].setValue(newVal);
  if (d->Settings->status() != QSettings::NoError)
    {
    logger.warn( QString("Error %1 while writing setting %1")
      .arg(static_cast<int>(d->Settings->status()))
      .arg(key));
    }
  if (oldVal != newVal)
    {
    emit settingChanged(key, newVal);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::registerProperty(const QString& key,
                                        QObject* object,
                                        const QString& property,
                                        const char* signal)
{
  Q_D(ctkSettingsPanel);
  PropertyType prop;
  prop.Object = object;
  prop.Property = property;
  prop.DefaultValue = prop.PreviousValue = prop.value();

  if (d->Settings && d->Settings->contains(key))
    {
    QVariant val = d->Settings->value(key);
    prop.setValue(val);
    prop.PreviousValue = val;
    }
  d->Properties[key] = prop;

  d->SignalMapper->setMapping(object, key);
  this->connect(object, signal, d->SignalMapper, SLOT(map()));
  
  if (d->SaveToSettingsWhenRegister)
    {
    this->updateSetting(key);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::applySettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    PropertyType& prop = d->Properties[key];
    prop.PreviousValue = prop.value();
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::resetSettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    this->setSetting(key, d->Properties[key].PreviousValue);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::restoreDefaultSettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    this->setSetting(key, d->Properties[key].DefaultValue);
    }
}
