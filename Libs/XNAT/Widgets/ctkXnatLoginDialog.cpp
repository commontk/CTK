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

#include "ctkXnatLoginDialog.h"

#include <QListView>
#include <QMap>
#include <QMessageBox>
#include <QStringListModel>
#include <QTimer>

#include <ctkXnatConnection.h>
#include <ctkXnatException.h>
#include "ctkXnatLoginProfile.h"
#include "ctkXnatSettings.h"

class ctkXnatLoginDialogPrivate
{
public:
  ctkXnatLoginDialogPrivate(ctkXnatConnectionFactory& f)
  : factory(f)
  {
  }

  ctkXnatSettings* settings;

  ctkXnatConnectionFactory& factory;
  ctkXnatConnection* connection;

  QMap<QString, ctkXnatLoginProfile*> profiles;

  QStringListModel model;
  QStringList profileNames;

  bool dirty;
};

ctkXnatLoginDialog::ctkXnatLoginDialog(ctkXnatConnectionFactory& f, QWidget* parent, Qt::WindowFlags flags)
: QDialog(parent, flags)
, ui(0)
, d_ptr(new ctkXnatLoginDialogPrivate(f))
{
  Q_D(ctkXnatLoginDialog);

  // initialize data members
  d->settings = 0;
  d->connection = 0;
  d->dirty = false;

  if (!ui)
    {
    // Create UI
    ui = new Ui::ctkXnatLoginDialog();
    ui->setupUi(this);

    QItemSelectionModel* oldSelectionModel = ui->lstProfiles->selectionModel();
    ui->lstProfiles->setModel(&d->model);
    delete oldSelectionModel;
    ui->lstProfiles->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->lstProfiles->setSelectionRectVisible(false);
    ui->lstProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->btnSave->setEnabled(false);

    // Create connections after setting defaults, so you don't trigger stuff when setting defaults.
    createConnections();
    }
}

ctkXnatLoginDialog::~ctkXnatLoginDialog()
{
  Q_D(ctkXnatLoginDialog);

  foreach (ctkXnatLoginProfile* profile, d->profiles)
    {
    delete profile;
    }

  if (ui)
    {
    delete ui;
    }
}

void ctkXnatLoginDialog::createConnections()
{
  connect(ui->edtProfileName, SIGNAL(textChanged(const QString&)), this, SLOT(onFieldChanged()));
  connect(ui->edtServerUri, SIGNAL(textChanged(const QString&)), this, SLOT(onFieldChanged()));
  connect(ui->edtUserName, SIGNAL(textChanged(const QString&)), this, SLOT(onFieldChanged()));
  // Password change is not listened to.
//  connect(ui->edtPassword, SIGNAL(textChanged(const QString&)), this, SLOT(onFieldChanged()));
  connect(ui->cbxDefaultProfile, SIGNAL(toggled(bool)), this, SLOT(onFieldChanged()));
  connect(ui->lstProfiles->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
      this, SLOT(onCurrentProfileChanged(const QModelIndex&)));
}

ctkXnatSettings* ctkXnatLoginDialog::settings() const
{
  Q_D(const ctkXnatLoginDialog);

  return d->settings;
}

void ctkXnatLoginDialog::setSettings(ctkXnatSettings* settings)
{
  Q_D(ctkXnatLoginDialog);
  d->settings = settings;
  d->profiles = d->settings->getLoginProfiles();

  d->profileNames = d->profiles.keys();
  d->profileNames.sort();
  d->model.setStringList(d->profileNames);
  
  ctkXnatLoginProfile* defaultProfile = d->settings->getDefaultLoginProfile();

  if (defaultProfile)
    {
    int profileNumber = d->profileNames.indexOf(defaultProfile->name());
    QModelIndex index = d->model.index(profileNumber);
    if (index.isValid())
      {
      ui->lstProfiles->setCurrentIndex(index);
      }
    ui->edtPassword->setFocus();
    }
}

ctkXnatConnection* ctkXnatLoginDialog::getConnection()
{
  Q_D(ctkXnatLoginDialog);
  return d->connection;
}

void ctkXnatLoginDialog::accept()
{
  Q_D(ctkXnatLoginDialog);

  QString url = ui->edtServerUri->text();
  if ( url.isEmpty() )
    {
    QMessageBox::warning(this, tr("Missing XNAT server URI"), tr("Please enter XNAT server URI."));
    ui->edtServerUri->selectAll();
    ui->edtServerUri->setFocus();
    return;
    }

  QString userName = ui->edtUserName->text();
  if ( userName.isEmpty() )
    {
    QMessageBox::warning(this, tr("Missing user name"), tr("Please enter user name."));
    ui->edtUserName->selectAll();
    ui->edtUserName->setFocus();
    return;
    }

  if (d->dirty)
    {
    const QString& profileName = ui->edtProfileName->text();
    if (askToSaveProfile(profileName))
      {
      saveProfile(profileName);
      }
    }

  QString password = ui->edtPassword->text();

  // create XNAT connection
  try
    {
    d->connection = d->factory.makeConnection(url.toAscii().constData(), userName.toAscii().constData(),
                                        password.toAscii().constData());
    }
  catch (ctkXnatException& e)
    {
    QMessageBox::warning(this, tr("Invalid Login Error"), tr(e.what()));
    ui->edtServerUri->selectAll();
    ui->edtServerUri->setFocus();
    return;
    }

  QDialog::accept();
}

void ctkXnatLoginDialog::onCurrentProfileChanged(const QModelIndex& currentIndex)
{
  Q_D(ctkXnatLoginDialog);

  if (!currentIndex.isValid())
    {
    loadProfile();
    return;
    }

  int originalIndexRow = currentIndex.row();
  QString newProfileName = d->profileNames[currentIndex.row()];
  ctkXnatLoginProfile* profile = d->profiles[newProfileName];

  bool newProfileSaved = false;
  if (d->dirty)
    {
    QString profileName = ui->edtProfileName->text();
    if (askToSaveProfile(profileName))
      {
      saveProfile(profileName);
      newProfileSaved = true;
      }
    }

  loadProfile(*profile);

  d->dirty = false;
  ui->btnSave->setEnabled(false);
  ui->btnDelete->setEnabled(true);

  // Ugly hack. If the current index has changed because of saving the edited element
  // then we have to select it again, but a bit later. If we select it right here then
  // both the original index and the new index are selected. (Even if single selection
  // is set.)
  if (newProfileSaved && originalIndexRow != currentIndex.row())
    {
    QTimer::singleShot(0, this, SLOT(resetLstProfilesCurrentIndex()));
    }
}

void ctkXnatLoginDialog::resetLstProfilesCurrentIndex()
{
  // Yes, this is really needed. See the comment above.
  ui->lstProfiles->setCurrentIndex(ui->lstProfiles->currentIndex());
}

bool ctkXnatLoginDialog::askToSaveProfile(const QString& profileName)
{
  QString question = QString(
      "You have not saved the changes of the %1 profile.\n"
      "Do you want to save them now?").arg(profileName);
  QMessageBox::StandardButton answer = QMessageBox::question(this, "", question, QMessageBox::Yes | QMessageBox::No,
                                QMessageBox::Yes);

  return answer == QMessageBox::Yes;
}

void ctkXnatLoginDialog::saveProfile(const QString& profileName)
{
  Q_D(ctkXnatLoginDialog);
  
  ctkXnatLoginProfile* profile = d->profiles[profileName];
  bool oldProfileWasDefault = profile && profile->isDefault();
  if (!profile)
    {
    profile = new ctkXnatLoginProfile();
    d->profiles[profileName] = profile;
    int profileNumber = d->profileNames.size();
    
    // Insertion into the profile name list and the listView (ascending order)
    int idx = 0;
    while (idx < profileNumber && QString::localeAwareCompare(profileName, d->profileNames[idx]) > 0)
      {
      ++idx;
      }
    d->profileNames.insert(idx, profileName);
    d->model.insertRow(idx);
    d->model.setData(d->model.index(idx), profileName);
    }
  
  storeProfile(*profile);
  
  // If the profile is to be default then remove the default flag from the other profiles.
  // This code assumes that the newly created profiles are not default.
  if (profile->isDefault() && !oldProfileWasDefault)
    {
    foreach (ctkXnatLoginProfile* otherProfile, d->profiles.values())
      {
      const QString& otherProfileName = otherProfile->name();
      if (otherProfileName != profileName && otherProfile->isDefault())
        {
        otherProfile->setDefault(false);
        d->settings->setLoginProfile(otherProfileName, otherProfile);
        }
      }
    }
  
  d->settings->setLoginProfile(profileName, profile);
  d->dirty = false;
  ui->btnSave->setEnabled(false);
}

void ctkXnatLoginDialog::on_btnSave_clicked()
{
  Q_D(ctkXnatLoginDialog);
  QString editedProfileName = ui->edtProfileName->text();

  bool selectSavedProfile = true;
  
  QModelIndex currentIndex = ui->lstProfiles->currentIndex();
  if (currentIndex.isValid())
    {
    QString selectedProfileName = d->profileNames[currentIndex.row()];
    if (editedProfileName == selectedProfileName)
      {
      selectSavedProfile = false;
      }
    }

  saveProfile(editedProfileName);

  if (selectSavedProfile)
    {
    int editedProfileNumber = d->profileNames.indexOf(editedProfileName);
    QModelIndex editedProfileIndex = d->model.index(editedProfileNumber, 0);
    ui->lstProfiles->setCurrentIndex(editedProfileIndex);
    }
}

void ctkXnatLoginDialog::blockSignalsOfFields(bool value)
{
  ui->edtProfileName->blockSignals(value);
  ui->edtServerUri->blockSignals(value);
  ui->edtUserName->blockSignals(value);
  ui->edtPassword->blockSignals(value);
  ui->cbxDefaultProfile->blockSignals(value);
}

void ctkXnatLoginDialog::loadProfile(const ctkXnatLoginProfile& profile)
{
  blockSignalsOfFields(true);

  ui->edtProfileName->setText(profile.name());
  ui->edtServerUri->setText(profile.serverUri());
  ui->edtUserName->setText(profile.userName());
  ui->edtPassword->setText(profile.password());
  ui->cbxDefaultProfile->setChecked(profile.isDefault());

  blockSignalsOfFields(false);
}

void ctkXnatLoginDialog::storeProfile(ctkXnatLoginProfile& profile)
{
  profile.setName(ui->edtProfileName->text());
  profile.setServerUri(ui->edtServerUri->text());
  profile.setUserName(ui->edtUserName->text());
  profile.setPassword(ui->edtPassword->text());
  profile.setDefault(ui->cbxDefaultProfile->isChecked());
}

void ctkXnatLoginDialog::on_btnDelete_clicked()
{
  Q_D(ctkXnatLoginDialog);

  QString profileName = ui->edtProfileName->text();

  int idx = d->profileNames.indexOf(profileName);
  d->model.removeRow(idx);
  d->profileNames.removeAt(idx);
  delete d->profiles.take(profileName);

  if (d->profiles.empty())
    {
    ui->btnDelete->setEnabled(false);
    ui->edtProfileName->setFocus();
    }
  
  d->settings->removeLoginProfile(profileName);
}

void ctkXnatLoginDialog::on_edtProfileName_textChanged(const QString& text)
{
  ui->lstProfiles->clearSelection();
  ui->btnDelete->setEnabled(false);
}

void ctkXnatLoginDialog::onFieldChanged()
{
  Q_D(ctkXnatLoginDialog);
  d->dirty = true;
  ui->btnSave->setEnabled(true);
}
