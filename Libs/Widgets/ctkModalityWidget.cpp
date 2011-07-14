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
#include <QStringList>

// CTK includes
#include "ctkModalityWidget.h"
#include "ui_ctkModalityWidget.h"
#include "ctkLogger.h"
#include "ctkFlowLayout.h"

// STD includes
#include <cmath>

static ctkLogger logger("org.commontk.libs.widgets.ctkModalityWidget");

QStringList sDefaultVisibleModalities;

//-----------------------------------------------------------------------------
class ctkModalityWidgetPrivate: public Ui_ctkModalityWidget
{
  Q_DECLARE_PUBLIC(ctkModalityWidget);
protected:
  ctkModalityWidget* const q_ptr;

public:
  ctkModalityWidgetPrivate(ctkModalityWidget& object);
  void init();
  void updateAnyCheckBoxState();
  
  QStringList SelectedModalities;
  QStringList VisibleModalities;
  QMap<QString, QCheckBox*> Modalities;
};

// --------------------------------------------------------------------------
ctkModalityWidgetPrivate::ctkModalityWidgetPrivate(ctkModalityWidget& object)
  :q_ptr(&object)
{
  if (sDefaultVisibleModalities.isEmpty())
    {
    sDefaultVisibleModalities << "CT" << "MR" << "US" << "CR" << "XA" << "NM" << "PT";
    }
}

// --------------------------------------------------------------------------
void ctkModalityWidgetPrivate::init()
{
  Q_Q(ctkModalityWidget);
  this->setupUi(q);
  
  this->AnyCheckBox->setTristate(true);
  QObject::connect(this->AnyCheckBox, SIGNAL(stateChanged(int)),
                   q, SLOT(onAnyChanged(int)));

  foreach(QCheckBox* box, q->findChildren<QCheckBox*>())
    {
    if (box == this->AnyCheckBox)
      {
      continue;
      }
    this->Modalities[box->text()] = box;
    QObject::connect(box, SIGNAL(toggled(bool)),
                     q, SLOT(onModalityChecked(bool)));
    }

  // reparent items into a flow layout
  QLayout* layout = q->layout();
  ctkFlowLayout* flowLayout = new ctkFlowLayout(Qt::Vertical);
  flowLayout->setContentsMargins(layout->contentsMargins());
  flowLayout->setPreferredExpandingDirections(Qt::Vertical);
  QLayoutItem* item;
  while ((item = layout->takeAt(0)))
    {
    flowLayout->addItem(item);
    }
  delete layout;
  q->setLayout(flowLayout);
  
  q->setVisibleModalities(sDefaultVisibleModalities);
  QSize sizeHint = flowLayout->sizeHint();
  double area = sizeHint.width() * sizeHint.height();
  area = sqrt(area);
  sizeHint = QSize(area, area);
  q->resize(sizeHint);
  q->selectAll();
}

// --------------------------------------------------------------------------
void ctkModalityWidgetPrivate::updateAnyCheckBoxState()
{
  Q_Q(ctkModalityWidget);
  if (this->SelectedModalities.isEmpty())
    {
    this->AnyCheckBox->setCheckState(Qt::Unchecked);
    }
  else if (q->areAllModalitiesSelected())
    {
    this->AnyCheckBox->setCheckState(Qt::Checked);
    }
  else
    {
    this->AnyCheckBox->setCheckState(Qt::PartiallyChecked);
    }
}

// --------------------------------------------------------------------------
ctkModalityWidget::ctkModalityWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkModalityWidgetPrivate(*this))
{
  Q_D(ctkModalityWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkModalityWidget::~ctkModalityWidget()
{
}

// --------------------------------------------------------------------------
QStringList ctkModalityWidget::selectedModalities()const
{
  Q_D(const ctkModalityWidget);
  return d->SelectedModalities;
}

// --------------------------------------------------------------------------
void ctkModalityWidget::setSelectedModalities(const QStringList& modalities)
{
  if (modalities == this->selectedModalities())
    {
    return;
    }
  bool blocked = this->blockSignals(true);
  this->unselectAll();
  foreach(QString modality, modalities)
    {
    this->selectModality(modality);
    }
  this->blockSignals(blocked);
  emit this->selectedModalitiesChanged(modalities);
}

// --------------------------------------------------------------------------
QStringList ctkModalityWidget::visibleModalities()const
{
  Q_D(const ctkModalityWidget);
  return d->VisibleModalities;
}

// --------------------------------------------------------------------------
void ctkModalityWidget::setVisibleModalities(const QStringList& modalities)
{
  if (modalities == this->visibleModalities())
    {
    return;
    }
  this->hideAll();
  foreach(QString modality, modalities)
    {
    this->showModality(modality);
    }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::selectModality(const QString& modality, bool select)
{
  Q_D(ctkModalityWidget);
  QCheckBox* modalityBox = d->Modalities[modality];
  modalityBox->setChecked(select);
}

// --------------------------------------------------------------------------
void ctkModalityWidget::showModality(const QString& modality, bool show)
{
  Q_D(ctkModalityWidget);
  QCheckBox* modalityBox = d->Modalities[modality];
  modalityBox->setVisible(show);
}

// --------------------------------------------------------------------------
void ctkModalityWidget::selectAll()
{
  Q_D(ctkModalityWidget);
  if (this->areAllModalitiesSelected())
    {
    return;
    }
  bool blocked = this->blockSignals(true);
  foreach(const QString& modality, d->Modalities.keys())
    {
    this->selectModality(modality, true);
    }
  this->blockSignals(blocked);
  emit this->selectedModalitiesChanged(d->SelectedModalities);
}

// --------------------------------------------------------------------------
void ctkModalityWidget::unselectAll()
{
  Q_D(ctkModalityWidget);
  if (d->SelectedModalities.count() == 0)
    {
    return;
    }
  bool blocked = this->blockSignals(true);
  foreach(const QString& modality, d->Modalities.keys())
    {
    this->selectModality(modality, false);
    }
  this->blockSignals(blocked);
  emit this->selectedModalitiesChanged(d->SelectedModalities);
}

// --------------------------------------------------------------------------
void ctkModalityWidget::showAll()
{
  Q_D(ctkModalityWidget);
  foreach(const QString& modality, d->Modalities.keys())
    {
    this->showModality(modality, true);
    }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::hideAll()
{
  Q_D(ctkModalityWidget);
  foreach(const QString& modality, d->Modalities.keys())
    {
    this->showModality(modality, false);
    }
}

// --------------------------------------------------------------------------
bool ctkModalityWidget::areAllModalitiesSelected()const
{
  Q_D(const ctkModalityWidget);
  return d->SelectedModalities.count() == d->Modalities.count();
}

// --------------------------------------------------------------------------
bool ctkModalityWidget::areAllModalitiesVisible()const
{
  Q_D(const ctkModalityWidget);
  return d->VisibleModalities.count() == d->Modalities.count();
}

// --------------------------------------------------------------------------
QStringList ctkModalityWidget::modalities()const
{
  Q_D(const ctkModalityWidget);
  return d->Modalities.keys();
}

// --------------------------------------------------------------------------
void ctkModalityWidget::onAnyChanged(int state)
{
  if (state == Qt::Unchecked)
    {
    this->unselectAll();
    }
  else if (state == Qt::Checked)
    {
    this->selectAll();
    }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::onModalityChecked(bool checked)
{
  Q_D(ctkModalityWidget);
  QCheckBox* box = qobject_cast<QCheckBox*>(this->sender());
  QString modality = box->text();
  if (checked)
    {
    d->SelectedModalities.append(modality);
    }
  else
    {
    d->SelectedModalities.removeAll(modality);
    }
  d->updateAnyCheckBoxState();    
  emit this->selectedModalitiesChanged(d->SelectedModalities);
}
