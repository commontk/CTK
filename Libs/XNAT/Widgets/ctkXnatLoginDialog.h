/*=============================================================================

  Library: CTK

  Copyright (c) University College London,
    Centre for Medical Image Computing

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

#ifndef ctkXnatLoginDialog_h
#define ctkXnatLoginDialog_h

#include <QDialog>

#include "ctkXNATWidgetsExport.h"

#include "ui_ctkXnatLoginDialog.h"

#include "ctkXnatLoginProfile.h"

class ctkXnatConnection;
class ctkXnatConnectionFactory;
class ctkXnatLoginDialogPrivate;
class ctkXnatSettings;

class CTK_XNAT_WIDGETS_EXPORT ctkXnatLoginDialog : public QDialog
{
  Q_OBJECT

public:
  explicit ctkXnatLoginDialog(ctkXnatConnectionFactory* f, QWidget* parent = 0, Qt::WindowFlags flags = 0);
  virtual ~ctkXnatLoginDialog();

  ctkXnatSettings* settings() const;
  void setSettings(ctkXnatSettings* settings);

  ctkXnatConnection* getConnection();

  virtual void accept();

private slots:

  void on_btnSave_clicked();
  void on_btnDelete_clicked();
  void on_edtProfileName_textChanged(const QString& text);
  void onFieldChanged();
  void onCurrentProfileChanged(const QModelIndex& current);
  void resetLstProfilesCurrentIndex();

private:
  void createConnections();
  void blockSignalsOfFields(bool value);

  void saveProfile(const QString& profileName);
  bool askToSaveProfile(const QString& profileName);
  void loadProfile(const ctkXnatLoginProfile& profile = ctkXnatLoginProfile());
  void storeProfile(ctkXnatLoginProfile& profile);

  /// \brief All the controls for the main view part.
  Ui::ctkXnatLoginDialog* ui;

  /// \brief d pointer of the pimpl pattern
  QScopedPointer<ctkXnatLoginDialogPrivate> d_ptr;

  Q_DECLARE_PRIVATE(ctkXnatLoginDialog);
  Q_DISABLE_COPY(ctkXnatLoginDialog);
};

#endif
