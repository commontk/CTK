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

#ifndef __ctkSettingsPanel_h
#define __ctkSettingsPanel_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class QSettings;
class ctkSettingsPanelPrivate;

class CTK_WIDGETS_EXPORT ctkSettingsPanel : public QWidget
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit ctkSettingsPanel(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkSettingsPanel();

  QSettings* settings()const;
  void setSettings(QSettings* settings);

  /// Add an entry into the settings uniquely defined by the \a key name and the
  /// current value of the property.
  /// The property is then synchronized with the settings by observing the signal
  /// notification. Anytime the property is modified (the signal \a signal is
  /// fired), its value associated to \a key is updated in the settings.
  /// \a signal is typically the value under NOTIFY in Q_PROPERTY.
  /// The current value of the property is later used when
  /// restoreDefaultSettings() is called.
  /// \sa Q_PROPERTY()
  void registerProperty(const QString& key,
                        QObject* object,
                        const QString& property,
                        const char* signal);
  /// Set the setting to the property defined by the key.
  /// The old value can be restored using resetSettings()
  void setSetting(const QString& key, const QVariant& newVal);

public slots:

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

signals:
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

protected slots:
  void updateSetting(const QString& key);

protected:
  QScopedPointer<ctkSettingsPanelPrivate> d_ptr;

  virtual void updateProperties();
private:
  Q_DECLARE_PRIVATE(ctkSettingsPanel);
  Q_DISABLE_COPY(ctkSettingsPanel);
};

#endif
