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

#ifndef __ctkSettingsPanel_h
#define __ctkSettingsPanel_h

// Qt includes
#include <QMetaType>
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class QSettings;
class ctkSettingsPanelPrivate;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkSettingsPanel : public QWidget
{
  Q_OBJECT
  Q_ENUMS(SettingOption)
  Q_FLAGS(SettingOptions)

  Q_PROPERTY(QSettings* settings READ settings WRITE setSettings);

public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit ctkSettingsPanel(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkSettingsPanel();

  QSettings* settings()const;
  void setSettings(QSettings* settings);

  enum SettingOption{
    OptionNone = 0x0000,
    OptionRequireRestart = 0x0001,
    OptionAll_Mask = ~0
  };
  Q_DECLARE_FLAGS(SettingOptions, SettingOption)
  /// Add an entry into the settings uniquely defined by the \a key name and the
  /// current value of the property.
  /// The property is then synchronized with the settings by observing the signal
  /// notification. Anytime the property is modified (the signal \a signal is
  /// fired), its value associated to \a key is updated in the settings.
  /// \a signal is typically the value under NOTIFY in Q_PROPERTY.
  /// The current value of the property is later used when
  /// restoreDefaultSettings() is called.
  /// If you want to register the logical complement of a boolean property
  /// you can use ctkBooleanMapper:
  /// <code>
  /// panel->registerProperty("unchecked",
  ///                         new ctkBooleanMapper(checkBox, "checked", SIGNAL(toggled(bool))),
  ///                         "complement", SIGNAL(complementChanged(bool)));
  /// </code>
  /// By default, property are associated with the general settings set using setSettings(QSettings*)
  /// or ctkSettingsDialog::setSettings(QSettings*). Note that it also possible to associate
  /// a specific \a settings for any given \a settingKey.
  /// \sa Q_PROPERTY(), \sa ctkBooleanMapper
  void registerProperty(const QString& settingKey,
                        QObject* object,
                        const QString& objectProperty,
                        const char* propertySignal,
                        const QString& settingLabel = QString(),
                        SettingOptions options = OptionNone,
                        QSettings * settings = 0);

  /// \copybrief registerProperty
  /// \overload
  Q_INVOKABLE void registerProperty(const QString& settingKey, QObject* object,
                                    const QString& objectProperty,
                                    const QByteArray& propertySignal,
                                    const QString& settingLabel = QString(),
                                    SettingOptions options = OptionNone,
                                    QSettings * settings = 0);

  /// Set the setting to the property defined by the key.
  /// The old value can be restored using resetSettings()
  void setSetting(const QString& key, const QVariant& newVal);

  /// Return the list of settings keys that have been modified and are
  /// not yet applied.
  QStringList changedSettings()const;

  /// Return the label associated to a setting
  QString settingLabel(const QString& settingKey)const;

  /// Return the options associated to a setting
  SettingOptions settingOptions(const QString& settingKey)const;
public Q_SLOTS:

  /// Forget the old property values so next time resetSettings is called it
  /// will set the properties with the same values when applySettings() is
  /// called.
  virtual void applySettings();

  /// Restore all the properties with their values when applySettings() was
  /// called last (or their original values if applySettings was never called).
  virtual void resetSettings();

  /// Restore all the properties with their original values; the current values
  /// of the properties when they were registered using registerProperty().
  virtual void restoreDefaultSettings();

  /// Reload all properties from disk.
  ///
  /// This reloads all properties from their respective QSettings instance(s).
  /// The previous values are discarded (as in resetSettings()).
  /// \sa resetSettings(), restoreDefaultSettings()
  virtual void reloadSettings();

Q_SIGNALS:
  /// Fired anytime a property is modified.
  void settingChanged(const QString& key, const QVariant& value);

protected:
  /// Return the default value of a property identified by its settings \a key
  /// \sa registerProperty();
  QVariant defaultPropertyValue(const QString& key) const;

  /// Return the previous value of a property identified by its settings \a key
  /// \sa registerProperty();
  QVariant previousPropertyValue(const QString& key) const;

  /// Return the value of a property identified by its settings \a key
  /// \sa registerProperty();
  QVariant propertyValue(const QString& key) const;

protected Q_SLOTS:
  void updateSetting(const QString& key);

protected:
  QScopedPointer<ctkSettingsPanelPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSettingsPanel);
  Q_DISABLE_COPY(ctkSettingsPanel);
};

Q_DECLARE_METATYPE(ctkSettingsPanel::SettingOption)
Q_DECLARE_METATYPE(ctkSettingsPanel::SettingOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(ctkSettingsPanel::SettingOptions)

#endif
