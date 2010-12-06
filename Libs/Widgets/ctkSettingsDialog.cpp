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
#include <QMap>
#include <QPushButton>
#include <QSettings>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkSettingsDialog.h"
#include "ui_ctkSettingsDialog.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkSettingsDialog");

//-----------------------------------------------------------------------------
class ctkSettingsDialogPrivate: public Ui_ctkSettingsDialog
{
  Q_DECLARE_PUBLIC(ctkSettingsDialog);
protected:
  ctkSettingsDialog* const q_ptr;
public:
  ctkSettingsDialogPrivate(ctkSettingsDialog& object);
  void init();

  ctkSettingsPanel* panel(QTreeWidgetItem* item)const;
  QTreeWidgetItem* item(ctkSettingsPanel* panel)const;
  QTreeWidgetItem* item(const QString& label)const;

  void beginGroup(ctkSettingsPanel* panel);
  void endGroup(ctkSettingsPanel* panel);

  QSettings* Settings;

protected:
  QMap<QTreeWidgetItem*, ctkSettingsPanel*> Panels;
};

// --------------------------------------------------------------------------
ctkSettingsDialogPrivate::ctkSettingsDialogPrivate(ctkSettingsDialog& object)
  :q_ptr(&object)
{
  this->Settings = 0;
}

// --------------------------------------------------------------------------
void ctkSettingsDialogPrivate::init()
{
  Q_Q(ctkSettingsDialog);
  this->setupUi(q);

  QObject::connect(this->SettingsTreeWidget,
    SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    q, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
  QObject::connect(this->SettingsButtonBox, SIGNAL(clicked(QAbstractButton*)),
                   q, SLOT(onDialogButtonClicked(QAbstractButton*)));
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsDialogPrivate::panel(QTreeWidgetItem* item)const
{
  return this->Panels.value(item, 0);
}

// --------------------------------------------------------------------------
QTreeWidgetItem* ctkSettingsDialogPrivate::item(ctkSettingsPanel* panel)const
{
  return this->Panels.key(panel, this->SettingsTreeWidget->invisibleRootItem());
}

// --------------------------------------------------------------------------
QTreeWidgetItem* ctkSettingsDialogPrivate::item(const QString& label)const
{
  QMap<QTreeWidgetItem*, ctkSettingsPanel*>::const_iterator it;
  for (it = this->Panels.constBegin(); it != this->Panels.constEnd(); ++it)
    {
    if (it.value()->windowTitle() == label)
      {
      return it.key();
      }
    }
  return this->SettingsTreeWidget->invisibleRootItem();
}

// --------------------------------------------------------------------------
ctkSettingsDialog::ctkSettingsDialog(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkSettingsDialogPrivate(*this))
{
  Q_D(ctkSettingsDialog);
  d->init();
}

// --------------------------------------------------------------------------
ctkSettingsDialog::~ctkSettingsDialog()
{
}

// --------------------------------------------------------------------------
QSettings* ctkSettingsDialog::settings()const
{
  Q_D(const ctkSettingsDialog);
  return d->Settings;
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::setSettings(QSettings* settings)
{
  Q_D(ctkSettingsDialog);

  d->Settings = settings;
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->setSettings(settings);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::addPanel(ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel)
{
  Q_D(ctkSettingsDialog);
  QTreeWidgetItem* newPanelItem = new QTreeWidgetItem;
  newPanelItem->setText(0, panel->windowTitle());
  newPanelItem->setIcon(0, panel->windowIcon());

  d->Panels[newPanelItem] = panel;
  QTreeWidgetItem* parentItem = d->item(parentPanel);
  parentItem->addChild(newPanelItem);
  d->SettingsStackedWidget->addWidget(panel);

  connect(panel, SIGNAL(settingChanged(const QString&, const QVariant&)),
          this, SLOT(onSettingChanged(const QString&, const QVariant&)));
  panel->setSettings(this->settings());
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::addPanel(const QString& label, ctkSettingsPanel* panel, 
           ctkSettingsPanel* parentPanel)
{
  panel->setWindowTitle(label);
  this->addPanel(panel, parentPanel);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::addPanel(const QString& label, const QIcon& icon,
           ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel)
{
  panel->setWindowTitle(label);
  panel->setWindowIcon(icon);
  this->addPanel(panel, parentPanel);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::setCurrentPanel(ctkSettingsPanel* panel)
{
  Q_D(ctkSettingsDialog);
  // eventually calls onCurrentItemChanged() where all the work is done
  d->SettingsTreeWidget->setCurrentItem(d->item(panel));
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::setCurrentPanel(const QString& label)
{
  Q_D(ctkSettingsDialog);
  // eventually calls onCurrentItemChanged() where all the work is done
  d->SettingsTreeWidget->setCurrentItem(d->item(label));
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsDialog::currentPanel()const
{
  Q_D(const ctkSettingsDialog);
  return d->panel(d->SettingsTreeWidget->currentItem());
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsDialog::panel(const QString& label)const
{
  Q_D(const ctkSettingsDialog);
  foreach(ctkSettingsPanel* settingsPanel, d->Panels.values())
    {
    if (settingsPanel->windowTitle() == label)
      {
      return settingsPanel;
      }
    }
  return 0;
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::accept()
{
  this->applySettings();
  this->Superclass::accept();
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::reject()
{
  this->resetSettings();
  this->Superclass::accept();
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::applySettings()
{
  Q_D(ctkSettingsDialog);
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->applySettings();
    }
  d->SettingsButtonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::resetSettings()
{
  Q_D(ctkSettingsDialog);
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->resetSettings();
    }
  d->SettingsButtonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::restoreDefaultSettings()
{
  Q_D(ctkSettingsDialog);
  // The panels may not contain ALL the settings of the application,
  // for the ones we don't default value, the best is to clear all of them...
  if (d->Settings)
    {
    d->Settings->clear();
    }
  // ... and restore settings for the ones we can
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->restoreDefaultSettings();
    }
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::onSettingChanged(const QString& key, const QVariant& newVal)
{
  Q_D(ctkSettingsDialog);
  d->SettingsButtonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
  emit settingChanged(key, newVal);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::onCurrentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previousItem)
{
  Q_D(ctkSettingsDialog);
  Q_UNUSED(previousItem);
  d->SettingsStackedWidget->setCurrentWidget(
    d->panel(currentItem));
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::onDialogButtonClicked(QAbstractButton* button)
{
  Q_D(ctkSettingsDialog);
  switch (d->SettingsButtonBox->standardButton(button))
    {
    case QDialogButtonBox::Reset:
      this->resetSettings();
      break;
    case QDialogButtonBox::RestoreDefaults:
      this->restoreDefaultSettings();
      break;
    default:
      break;
    }
}
