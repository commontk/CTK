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
#include <QMap>
#include <QMessageBox>
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

  QList<ctkSettingsPanel*> panels()const;
  ctkSettingsPanel* panel(QTreeWidgetItem* item)const;
  QTreeWidgetItem* item(ctkSettingsPanel* panel)const;
  QTreeWidgetItem* item(const QString& label)const;

  void beginGroup(ctkSettingsPanel* panel);
  void endGroup(ctkSettingsPanel* panel);

  void updatePanelTitle(ctkSettingsPanel* panel);
  void updateRestartRequiredLabel();

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

  this->SettingsButtonBox->button(QDialogButtonBox::Ok)->setToolTip(
    q->tr("Apply settings and close dialog."));
  this->SettingsButtonBox->button(QDialogButtonBox::Cancel)->setToolTip(
    q->tr("Reject settings changes and close dialog."));
  this->SettingsButtonBox->button(QDialogButtonBox::Reset)->setToolTip(
    q->tr("Reset settings to their values when the dialog opened"));
  this->SettingsButtonBox->button(QDialogButtonBox::RestoreDefaults)->setToolTip(
    q->tr("Restore settings to their default values."
    "To cancel a \"Restore\", you can \"Reset\" the settings."));
  q->setResetButton(false);

  q->setSettings(new QSettings(q));

  QObject::connect(this->SettingsTreeWidget,
    SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
    q, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));

  QObject::connect(this->SettingsTreeWidget, SIGNAL(expanded(QModelIndex)),
    q, SLOT(adjustTreeWidgetToContents()));
  QObject::connect(this->SettingsTreeWidget, SIGNAL(collapsed(QModelIndex)),
    q, SLOT(adjustTreeWidgetToContents()));

  QObject::connect(this->SettingsButtonBox, SIGNAL(clicked(QAbstractButton*)),
                   q, SLOT(onDialogButtonClicked(QAbstractButton*)));

  this->updateRestartRequiredLabel();
  q->adjustTreeWidgetToContents();
}

// --------------------------------------------------------------------------
QList<ctkSettingsPanel*> ctkSettingsDialogPrivate::panels()const
{
  return this->Panels.values();
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
void ctkSettingsDialogPrivate::updatePanelTitle(ctkSettingsPanel* panel)
{
  QTreeWidgetItem* panelItem = this->item(panel);
  QString title = panelItem->text(0);
  title.replace(QRegExp("\\*$"),"");
  if (!panel->changedSettings().isEmpty())
    {
    title.append('*');
    }
  panelItem->setText(0,title);
}

// --------------------------------------------------------------------------
void ctkSettingsDialogPrivate::updateRestartRequiredLabel()
{
  Q_Q(ctkSettingsDialog);
  QStringList restartRequiredSettings;
  foreach(const ctkSettingsPanel* panel, this->panels())
    {
    foreach(const QString& settingKey, panel->changedSettings())
      {
      if (panel->settingOptions(settingKey) & ctkSettingsPanel::OptionRequireRestart)
        {
        restartRequiredSettings << (panel->settingLabel(settingKey).isEmpty() ?
          settingKey : panel->settingLabel(settingKey));
        }
      }
    }
  bool restartRequired = !restartRequiredSettings.isEmpty();
  if (restartRequired)
    {
    QString header = q->tr(
      "<b style=\"color:red\">Restart required!</b><br>\n<small>"
      "The application must be restarted to take into account "
      "the new values of the following properties:\n");
    QString footer = q->tr("</small>");
    restartRequiredSettings.push_front(QString());
    this->RestartRequiredLabel->setText( header + restartRequiredSettings.join("<br>&nbsp;&nbsp;") + footer);
    }
  this->RestartRequiredLabel->setVisible(restartRequired);
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

  d->SettingsButtonBox->button(QDialogButtonBox::Reset)->setEnabled(false);

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

  this->adjustTreeWidgetToContents();

  connect(panel, SIGNAL(settingChanged(QString,QVariant)),
          this, SLOT(onSettingChanged(QString,QVariant)));
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
  bool emitRestartRequested = false;
  if (this->isRestartRequired())
    {
    QMessageBox::StandardButton answer = QMessageBox::question(this,"Restart required",
      "For settings to be taken into account, the application\n"
      "must be restarted. Restart the application now ?\n",
      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::No);
    if (answer == QMessageBox::Cancel)
      {
      return;
      }
    else
      {
      emitRestartRequested = (answer == QMessageBox::Yes);
      }
    }

  this->applySettings();
  this->Superclass::accept();
  if (emitRestartRequested)
    {
    emit restartRequested();
    }
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
void ctkSettingsDialog::reloadSettings()
{
  Q_D(ctkSettingsDialog);
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->reloadSettings();
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
  d->updatePanelTitle(qobject_cast<ctkSettingsPanel*>(this->sender()));
  d->updateRestartRequiredLabel();
  emit settingChanged(key, newVal);
}

// --------------------------------------------------------------------------
void ctkSettingsDialog
::onCurrentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previousItem)
{
  Q_D(ctkSettingsDialog);
  Q_UNUSED(previousItem);
  d->SettingsStackedWidget->setCurrentWidget(d->panel(currentItem));
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
      if (QMessageBox::warning(this,"Restore all settings",
            "Are you sure you want to reset\n"
            "all settings to their default values?\n",
            QMessageBox::RestoreDefaults, QMessageBox::Cancel)
          == QMessageBox::RestoreDefaults)
        {
        this->restoreDefaultSettings();
        }
      break;
    default:
      break;
    }
}

// --------------------------------------------------------------------------
void ctkSettingsDialog::adjustTreeWidgetToContents()
{
  Q_D(const ctkSettingsDialog);

  d->SettingsTreeWidget->resizeColumnToContents(0);
  #if (QT_VERSION >= QT_VERSION_CHECK(5,11,0))
  int character_pixel_width = d->SettingsTreeWidget->fontMetrics().horizontalAdvance('*');
  #else
  int character_pixel_width = d->SettingsTreeWidget->fontMetrics().width('*');
  #endif
  d->SettingsTreeWidget->setFixedWidth(
      qobject_cast<QAbstractItemView*>(d->SettingsTreeWidget)->sizeHintForColumn(0) +
      character_pixel_width +
      d->SettingsTreeWidget->indentation() / 2+
      2 * d->SettingsTreeWidget->frameWidth());
}

// -------------------------------------------------------------------------
bool ctkSettingsDialog::event(QEvent* event)
{
  if (event->type() == QEvent::FontChange ||
      event->type() == QEvent::StyleChange)
    {
    this->adjustTreeWidgetToContents();
    }
  return this->Superclass::event(event);
}

// -------------------------------------------------------------------------
bool ctkSettingsDialog::resetButton()const
{
  Q_D(const ctkSettingsDialog);
  return d->SettingsButtonBox->button(QDialogButtonBox::Reset)->isVisibleTo(
    const_cast<QDialogButtonBox*>(d->SettingsButtonBox));
}

// -------------------------------------------------------------------------
void ctkSettingsDialog::setResetButton(bool show)
{
  Q_D(ctkSettingsDialog);
  d->SettingsButtonBox->button(QDialogButtonBox::Reset)->setVisible(show);
}

// --------------------------------------------------------------------------
bool ctkSettingsDialog::isRestartRequired()const
{
  Q_D(const ctkSettingsDialog);
  return d->RestartRequiredLabel->isVisibleTo(
    const_cast<ctkSettingsDialog*>(this));
}
