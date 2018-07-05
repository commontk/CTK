/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QMetaProperty>
#include <QSettings>
#include <QSignalMapper>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkSettingsPanel");

namespace
{

// --------------------------------------------------------------------------
struct PropertyType
{
  typedef PropertyType Self;
  PropertyType();
  QObject* Object;
  QString  Property;
  QVariant DefaultValue;
  QString  Label;
  QSettings* Settings;
  ctkSettingsPanel::SettingOptions Options;

  QVariant value()const;
  bool setValue(const QVariant& value);

  QVariant previousValue()const;
  void setPreviousValue(const QVariant& value);

  QMetaProperty metaProperty();

  /// Workaround https://bugreports.qt.io/browse/QTBUG-19823
  static QVariant fixEmptyStringListVariant(const QVariant& value, const char* _typename);

private:

  QVariant PreviousValue;
};

// --------------------------------------------------------------------------
PropertyType::PropertyType()
  : Object(0), Settings(0)
  , Options(ctkSettingsPanel::OptionNone)
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
  QVariant value(val);
  value = Self::fixEmptyStringListVariant(value,
                                          this->metaProperty().typeName());
  bool success = true;
  if (this->Object->property(this->Property.toLatin1()) != value)
    {
    success = this->Object->setProperty(this->Property.toLatin1(), value);
    }
  Q_ASSERT(success);
  return success;
}

// --------------------------------------------------------------------------
QVariant PropertyType::previousValue()const
{
  return this->PreviousValue;
}

// --------------------------------------------------------------------------
void PropertyType::setPreviousValue(const QVariant& val)
{
  this->PreviousValue =
      Self::fixEmptyStringListVariant(val, this->metaProperty().typeName());
}

// --------------------------------------------------------------------------
QVariant PropertyType::fixEmptyStringListVariant(const QVariant& value,
                                                 const char* _typename)
{
  QVariant fixedValue = value;
  // HACK - See https://bugreports.qt.io/browse/QTBUG-19823
  if (qstrcmp(_typename, "QStringList") == 0 && !value.isValid())
    {
    fixedValue = QVariant(QStringList());
    }
  return fixedValue;
}

// --------------------------------------------------------------------------
QMetaProperty PropertyType::metaProperty()
{
  Q_ASSERT(this->Object);
  for(int i=0; i < this->Object->metaObject()->propertyCount(); ++i)
    {
    this->Object->metaObject()->property(i);
    if (this->Object->metaObject()->property(i).name() == this->Property)
      {
      return this->Object->metaObject()->property(i);
      }
    }
  return QMetaProperty();
}

} // end of anonymous namespace

//-----------------------------------------------------------------------------
class ctkSettingsPanelPrivate
{
  Q_DECLARE_PUBLIC(ctkSettingsPanel);
protected:
  ctkSettingsPanel* const q_ptr;

public:
  ctkSettingsPanelPrivate(ctkSettingsPanel& object);
  void init();

  /// Return QSettings associated with a given settingKey or the general \a Settings. 
  /// If \a settingKey is not found, it will return 0.
  /// \sa ctkSettingsPanel::registerProperty
  QSettings* settings(const QString& settingKey)const;

  QSettings*                  Settings;
  QMap<QString, PropertyType> Properties;
  bool                        SaveToSettingsWhenRegister;
};

// --------------------------------------------------------------------------
ctkSettingsPanelPrivate::ctkSettingsPanelPrivate(ctkSettingsPanel& object)
  :q_ptr(&object)
{
  qRegisterMetaType<ctkSettingsPanel::SettingOption>("ctkSettingsPanel::SettingOption");
  qRegisterMetaType<ctkSettingsPanel::SettingOptions>("ctkSettingsPanel::SettingOptions");
  this->Settings = 0;
  this->SaveToSettingsWhenRegister = true;
}

// --------------------------------------------------------------------------
void ctkSettingsPanelPrivate::init()
{
}

// --------------------------------------------------------------------------
QSettings* ctkSettingsPanelPrivate::settings(const QString& settingKey)const
{
  if (!this->Properties.contains(settingKey))
    {
    return 0;
    }
  const PropertyType& prop = this->Properties[settingKey];
  if (prop.Settings != 0)
    {
    return prop.Settings;
    }
  return this->Settings;
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
  this->reloadSettings();
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::reloadSettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    QSettings* settings = d->settings(key);
    if (!settings)
      {
      continue;
      }
    if (settings->contains(key))
      {
      QVariant value = settings->value(key);
      PropertyType& prop = d->Properties[key];
      // Update object registered using registerProperty()
      prop.setValue(value);
      prop.setPreviousValue(value);
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
  if (!d->settings(key))
    {
    return;
    }
  this->setSetting(key, d->Properties[key].value());
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::setSetting(const QString& key, const QVariant& newVal)
{
  Q_D(ctkSettingsPanel);
  QSettings* settings = d->settings(key);
  if (!settings)
    {
    return;
    }
  QVariant oldVal = settings->value(key);
  oldVal = PropertyType::fixEmptyStringListVariant(
        oldVal, d->Properties[key].metaProperty().typeName());
  settings->setValue(key, newVal);
  d->Properties[key].setValue(newVal);
  if (settings->status() != QSettings::NoError)
    {
    logger.warn( QString("Error #%1 while writing setting \"%2\"")
      .arg(static_cast<int>(settings->status()))
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
                                        const char* signal,
                                        const QString& label,
                                        ctkSettingsPanel::SettingOptions options,
                                        QSettings* settings)
{
  Q_D(ctkSettingsPanel);
  PropertyType prop;
  prop.Object = object;
  prop.Property = property;
  prop.setPreviousValue(prop.value());
  prop.DefaultValue = prop.previousValue();
  prop.Label = label;
  prop.Options = options;
  if (d->Settings != settings)
    {
    prop.Settings = settings;
    }
  
  QSettings* propSettings = settings ? settings : d->Settings;
  if (propSettings && propSettings->contains(key))
    {
    QVariant val = propSettings->value(key);
    prop.setValue(val);
    prop.setPreviousValue(val);
    }
    
  d->Properties[key] = prop;

  // Create a signal mapper per property to be able to support
  // multiple signals from the same sender.
  QSignalMapper* signalMapper = new QSignalMapper(this);
  QObject::connect(signalMapper, SIGNAL(mapped(QString)),
                   this, SLOT(updateSetting(QString)));
  signalMapper->setMapping(object, key);
  this->connect(object, signal, signalMapper, SLOT(map()));

  if (d->SaveToSettingsWhenRegister)
    {
    this->updateSetting(key);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::registerProperty(
  const QString& key, QObject* object, const QString& property,
  const QByteArray& signal, const QString& label,
  ctkSettingsPanel::SettingOptions options, QSettings* settings)
{
  this->registerProperty(key, object, property, signal.constData(),
                         label, options, settings);
}

// --------------------------------------------------------------------------
QVariant ctkSettingsPanel::defaultPropertyValue(const QString& key) const
{
  Q_D(const ctkSettingsPanel);
  if (!d->Properties.contains(key))
    {
    return QVariant();
    }
  return d->Properties.value(key).DefaultValue;
}

// --------------------------------------------------------------------------
QVariant ctkSettingsPanel::previousPropertyValue(const QString& key) const
{
  Q_D(const ctkSettingsPanel);
  if (!d->Properties.contains(key))
    {
    return QVariant();
    }
  return d->Properties.value(key).previousValue();
}

// --------------------------------------------------------------------------
QVariant ctkSettingsPanel::propertyValue(const QString& key) const
{
  Q_D(const ctkSettingsPanel);
  if (!d->Properties.contains(key))
    {
    return QVariant();
    }
  return d->Properties.value(key).value();
}

// --------------------------------------------------------------------------
QStringList ctkSettingsPanel::changedSettings()const
{
  Q_D(const ctkSettingsPanel);
  QStringList settingsKeys;
  foreach(const QString& key, d->Properties.keys())
    {
    const PropertyType& prop = d->Properties[key];
    if (prop.previousValue() != prop.value())
      {
      settingsKeys << key;
      }
    }
  return settingsKeys;
}

// --------------------------------------------------------------------------
QString ctkSettingsPanel::settingLabel(const QString& settingKey)const
{
  Q_D(const ctkSettingsPanel);
  return d->Properties[settingKey].Label;
}

// --------------------------------------------------------------------------
ctkSettingsPanel::SettingOptions ctkSettingsPanel
::settingOptions(const QString& settingKey)const
{
  Q_D(const ctkSettingsPanel);
  return d->Properties[settingKey].Options;
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::applySettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    PropertyType& prop = d->Properties[key];
    prop.setPreviousValue(prop.value());
    emit settingChanged(key, prop.value());
    }
}

// --------------------------------------------------------------------------
void ctkSettingsPanel::resetSettings()
{
  Q_D(ctkSettingsPanel);
  foreach(const QString& key, d->Properties.keys())
    {
    this->setSetting(key, d->Properties[key].previousValue());
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
