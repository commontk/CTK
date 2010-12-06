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

  void registerProperty(const QString& key,
                        QObject* object,
                        const QString& property,
                        const char* signal);
  void setSetting(const QString& key, const QVariant& newVal);

public slots:
  void applySettings();
  void resetSettings();
  void restoreDefaultSettings();

signals:
  void settingChanged(const QString& key, const QVariant& value);

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
