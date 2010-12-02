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

#ifndef __ctkSettingsWidget_h
#define __ctkSettingsWidget_h

// Qt includes
#include <QWidget>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkSettingsWidgetPrivate;
class QTreeWidgetItem;
class ctkSettingsPanel;
class QSettings;

class CTK_WIDGETS_EXPORT ctkSettingsWidget : public QWidget
{
  Q_OBJECT
public:
  /// Superclass typedef
  typedef QWidget Superclass;

  /// Constructor
  explicit ctkSettingsWidget(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkSettingsWidget();

  QSettings* settings()const;
  void setSettings(QSettings* settings);

  ctkSettingsPanel* panel(const QString& panel)const;
  ctkSettingsPanel* currentPanel()const;

  /// Uses the ctkSettingsPanel::windowTitle property to show in the list
  void addPanel(ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel = 0);
  /// Utility function 
  void addPanel(const QString& label, ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel = 0);
  void addPanel(const QString& label, const QIcon& icon, ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel = 0);

public slots:
  void setCurrentPanel(ctkSettingsPanel* panel);
  void setCurrentPanel(const QString& label);

signals:
  void settingChanged(const QString& key, const QVariant& value);

protected slots:
  void onCurrentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previous);

protected:
  QScopedPointer<ctkSettingsWidgetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSettingsWidget);
  Q_DISABLE_COPY(ctkSettingsWidget);
};

#endif
