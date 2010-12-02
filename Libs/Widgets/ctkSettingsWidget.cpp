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
#include <QSettings>

// CTK includes
#include "ctkSettingsPanel.h"
#include "ctkSettingsWidget.h"
#include "ui_ctkSettingsWidget.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkSettingsWidget");

//-----------------------------------------------------------------------------
class ctkSettingsWidgetPrivate: public Ui_ctkSettingsWidget
{
  Q_DECLARE_PUBLIC(ctkSettingsWidget);
protected:
  ctkSettingsWidget* const q_ptr;
public:
  ctkSettingsWidgetPrivate(ctkSettingsWidget& object);
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
ctkSettingsWidgetPrivate::ctkSettingsWidgetPrivate(ctkSettingsWidget& object)
  :q_ptr(&object)
{
  this->Settings = 0;
}

// --------------------------------------------------------------------------
void ctkSettingsWidgetPrivate::init()
{
  Q_Q(ctkSettingsWidget);
  this->setupUi(q);

  QObject::connect(this->SettingsTreeWidget,
    SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
    q, SLOT(onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsWidgetPrivate::panel(QTreeWidgetItem* item)const
{
  return this->Panels.value(item, 0);
}

// --------------------------------------------------------------------------
QTreeWidgetItem* ctkSettingsWidgetPrivate::item(ctkSettingsPanel* panel)const
{
  return this->Panels.key(panel, this->SettingsTreeWidget->invisibleRootItem());
}

// --------------------------------------------------------------------------
QTreeWidgetItem* ctkSettingsWidgetPrivate::item(const QString& label)const
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
ctkSettingsWidget::ctkSettingsWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkSettingsWidgetPrivate(*this))
{
  Q_D(ctkSettingsWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkSettingsWidget::~ctkSettingsWidget()
{
}

// --------------------------------------------------------------------------
QSettings* ctkSettingsWidget::settings()const
{
  Q_D(const ctkSettingsWidget);
  return d->Settings;
}

// --------------------------------------------------------------------------
void ctkSettingsWidget::setSettings(QSettings* settings)
{
  Q_D(ctkSettingsWidget);

  d->Settings = settings;
  foreach(ctkSettingsPanel* panel, d->Panels.values())
    {
    panel->setSettings(settings);
    }
}

// --------------------------------------------------------------------------
void ctkSettingsWidget
::addPanel(ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel)
{
  Q_D(ctkSettingsWidget);
  QTreeWidgetItem* newPanelItem = new QTreeWidgetItem;
  newPanelItem->setText(0, panel->windowTitle());
  newPanelItem->setIcon(0, panel->windowIcon());

  d->Panels[newPanelItem] = panel;
  QTreeWidgetItem* parentItem = d->item(parentPanel);
  parentItem->addChild(newPanelItem);
  d->SettingsStackedWidget->addWidget(panel);

  connect(panel, SIGNAL(settingChanged(const QString&, const QVariant&)),
          this, SIGNAL(settingChanged(const QString&, const QVariant&)));
  panel->setSettings(this->settings());
}

// --------------------------------------------------------------------------
void ctkSettingsWidget
::addPanel(const QString& label, ctkSettingsPanel* panel, 
           ctkSettingsPanel* parentPanel)
{
  panel->setWindowTitle(label);
  this->addPanel(panel, parentPanel);
}

// --------------------------------------------------------------------------
void ctkSettingsWidget
::addPanel(const QString& label, const QIcon& icon,
           ctkSettingsPanel* panel, ctkSettingsPanel* parentPanel)
{
  panel->setWindowTitle(label);
  panel->setWindowIcon(icon);
  this->addPanel(panel, parentPanel);
}

// --------------------------------------------------------------------------
void ctkSettingsWidget::setCurrentPanel(ctkSettingsPanel* panel)
{
  Q_D(ctkSettingsWidget);
  // eventually calls onCurrentItemChanged() where all the work is done
  d->SettingsTreeWidget->setCurrentItem(d->item(panel));
}

// --------------------------------------------------------------------------
void ctkSettingsWidget::setCurrentPanel(const QString& label)
{
  Q_D(ctkSettingsWidget);
  // eventually calls onCurrentItemChanged() where all the work is done
  d->SettingsTreeWidget->setCurrentItem(d->item(label));
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsWidget::currentPanel()const
{
  Q_D(const ctkSettingsWidget);
  return d->panel(d->SettingsTreeWidget->currentItem());
}

// --------------------------------------------------------------------------
ctkSettingsPanel* ctkSettingsWidget::panel(const QString& label)const
{
  Q_D(const ctkSettingsWidget);
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
void ctkSettingsWidget
::onCurrentItemChanged(QTreeWidgetItem* currentItem, QTreeWidgetItem* previousItem)
{
  Q_D(ctkSettingsWidget);
  Q_UNUSED(previousItem);
  d->SettingsStackedWidget->setCurrentWidget(
    d->panel(currentItem));
}
