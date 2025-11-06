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

#ifndef __ctkSettingsDialog_h
#define __ctkSettingsDialog_h

// Qt includes
#include <QDialog>
#include <QSettings>

// CTK includes
#include "ctkWidgetsExport.h"
#include "ctkSettingsPanel.h"

class QAbstractButton;
class QTreeWidgetItem;
class ctkSettingsDialogPrivate;

/// \ingroup Widgets
class CTK_WIDGETS_EXPORT ctkSettingsDialog : public QDialog
{
  Q_OBJECT
  /// This property controls whether the reset button is visible in the
  /// button box or not. The Cancel button is a reset button and closes
  /// the dialog at the same time.
  Q_PROPERTY(bool resetButton READ resetButton WRITE setResetButton);

  Q_PROPERTY(QSettings* settings READ settings WRITE setSettings);

  Q_PROPERTY(ctkSettingsPanel* currentPanel READ currentPanel WRITE setCurrentPanel);

  /// Specifies if a restart required to fully apply changes.
  ///
  /// This property is \c true if at least one OptionRestartRequired setting is
  /// changed. It doesn't imply that the user accepted to restart the
  /// application.
  Q_PROPERTY(bool restartRequired READ isRestartRequired);

public:
  /// Superclass typedef
  typedef QDialog Superclass;

  /// Constructor
  explicit ctkSettingsDialog(QWidget* parent = 0);

  /// Destructor
  virtual ~ctkSettingsDialog();

  QSettings* settings()const;
  void setSettings(QSettings* settings);

  /// Get the panel based on its title. The title is the string displayed to
  /// users therefore it is a localized string (translated to different languages).
  ctkSettingsPanel* panel(const QString& panel)const;
  ctkSettingsPanel* currentPanel()const;

  /// Add settings panel to the dialog.
  ///
  /// This adds the specified settings panel to the dialog. The panel's
  /// QWidget::windowTitle property is used as the panel name as shown in the
  /// panels list.
  Q_INVOKABLE void addPanel(ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel = 0);

  /// \copybrief addPanel
  ///
  /// This convenience overload allows the caller to specify the panel name
  /// that will be used in the panels list.
  Q_INVOKABLE void addPanel(const QString& label, ctkSettingsPanel* panel,
                            ctkSettingsPanel* parentPanel = 0);

  /// \copybrief addPanel
  ///
  /// This convenience overload allows the caller to specify the panel name
  /// that will be used in the panels list, as well as an icon for the panel.
  Q_INVOKABLE void addPanel(const QString& label, const QIcon& icon,
                            ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel = 0);

  bool resetButton()const;
  void setResetButton(bool show);

  /// \copybrief restartRequired
  /// \sa restartRequired, restartRequested
  bool isRestartRequired()const;

public Q_SLOTS:
  void setCurrentPanel(ctkSettingsPanel* panel);
  void setCurrentPanel(const QString& label);

  void applySettings();
  void resetSettings();
  void restoreDefaultSettings();

  /// Reload settings for all registered panels.
  ///
  /// This reloads the settings for all panels, effectively throwing out any
  /// values in the UI or the panels' caches and reverting to the values in the
  /// associated QSettings. You should call this if you have made changes to
  /// the QSettings that were not made through ctkSettingsPanel.
  void reloadSettings();

  virtual void accept();
  virtual void reject();

  /// Resize the left panel based on the panels titles.
  void adjustTreeWidgetToContents();

Q_SIGNALS:
  void settingChanged(const QString& key, const QVariant& value);
  /// Signal fired when the user accepts to restart the application because
  /// some OptionRestartRequired settings have changed.
  /// \sa isrestartRequired
  void restartRequested();

protected Q_SLOTS:
  void onSettingChanged(const QString& key, const QVariant& newVal);
  void onCurrentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previous);
  void onDialogButtonClicked(QAbstractButton* button);

protected:
  virtual bool event(QEvent *);

protected:
  QScopedPointer<ctkSettingsDialogPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkSettingsDialog);
  Q_DISABLE_COPY(ctkSettingsDialog);
};

#endif
