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

namespace
{
  QStringList sDefaultVisibleModalities;
}

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

  // Determine modality name from checkbox object name by removing "CheckBox" suffix
  QString modalityFromCheckBox(QCheckBox* modalityBox);

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
QString ctkModalityWidgetPrivate::modalityFromCheckBox(QCheckBox* modalityBox)
{
  const QString modalityCheckboxSuffix = "CheckBox";
  QString modalityName = modalityBox->objectName();  // example: RTImageCheckBox
  if (!modalityName.endsWith(modalityCheckboxSuffix))
  {
    return QString();
  }
  // Remove suffix
  modalityName.chop(modalityCheckboxSuffix.size());
  return modalityName;
}

// --------------------------------------------------------------------------
void ctkModalityWidgetPrivate::init()
{
  Q_Q(ctkModalityWidget);
  this->setupUi(q);

  this->AnyCheckBox->setTristate(true);
  QObject::connect(this->AnyCheckBox, SIGNAL(stateChanged(int)),
                   q, SLOT(onAnyChanged(int)));

  foreach(QCheckBox* modalityBox, q->findChildren<QCheckBox*>())
  {
    if (modalityBox == this->AnyCheckBox)
    {
      continue;
    }
    QString modalityName = this->modalityFromCheckBox(modalityBox);
    if (modalityName.isEmpty())
    {
      qWarning() << Q_FUNC_INFO << "failed to add checkbox: checkbox name" << modalityBox->objectName() << "is invalid and will be disabled";
      modalityBox->hide();
      continue;
    }
    this->Modalities[modalityName] = modalityBox;
    QObject::connect(modalityBox, SIGNAL(toggled(bool)),
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
#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
  QSignalBlocker blocker(this->AnyCheckBox);
#endif


  // None selected?
  bool foundSelectedModality = false;
  foreach(const QString & modality, this->Modalities.keys())
  {
    if (this->Modalities[modality]->isChecked())
    {
      foundSelectedModality = true;
      break;
    }
  }
  if (!foundSelectedModality)
  {
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
    bool wasBlocked = this->AnyCheckBox->blockSignals(true);
#endif
    this->AnyCheckBox->setCheckState(Qt::Unchecked);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
    this->AnyCheckBox->blockSignals(wasBlocked);
#endif
    return;
  }

  // All selected?
  bool allModalitiesSelected = true;
  foreach(const QString & modality, this->Modalities.keys())
  {
    if (!this->Modalities[modality]->isChecked())
    {
      allModalitiesSelected = false;
      break;
    }
  }
  if (allModalitiesSelected)
  {
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
    bool wasBlocked = this->AnyCheckBox->blockSignals(true);
#endif
    this->AnyCheckBox->setCheckState(Qt::Checked);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
    this->AnyCheckBox->blockSignals(wasBlocked);
#endif
    return;
  }

  // Some selected
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
  bool wasBlocked = this->AnyCheckBox->blockSignals(true);
#endif
  this->AnyCheckBox->setCheckState(Qt::PartiallyChecked);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
  this->AnyCheckBox->blockSignals(wasBlocked);
#endif
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
  QStringList modalities;
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    if (modalityBox->isChecked())
    {
      modalities << modality;
    }
  }
  return modalities;
}

// --------------------------------------------------------------------------
void ctkModalityWidget::setSelectedModalities(const QStringList& selectedModalities)
{
  Q_D(ctkModalityWidget);
  bool modified = false;
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    bool selected = selectedModalities.contains(modality);
    if (modalityBox->isChecked() != selected)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
      QSignalBlocker blocker(modalityBox);
#else
      bool wasBlocked = modalityBox->blockSignals(true);
#endif
      modalityBox->setChecked(selected);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
      modalityBox->blockSignals(wasBlocked);
#endif
      modified = true;
    }
  }
  if (modified)
  {
    d->updateAnyCheckBoxState();
    emit this->selectedModalitiesChanged(this->selectedModalities());
  }
}

// --------------------------------------------------------------------------
QStringList ctkModalityWidget::visibleModalities()const
{
  Q_D(const ctkModalityWidget);
  QStringList modalities;
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    // isHidden() means explicitly hidden (not the same as !isVisible())
    if (!modalityBox->isHidden())
    {
      modalities << modality;
    }
  }
  return modalities;
}

// --------------------------------------------------------------------------
void ctkModalityWidget::setVisibleModalities(const QStringList& visibleModalities)
{
  Q_D(ctkModalityWidget);
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    bool visible = visibleModalities.contains(modality);
    // isHidden() means explicitly hidden (not the same as !isVisible())
    if (modalityBox->isHidden() == visible)
    {
      modalityBox->setVisible(visible);
    }
  }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::selectModality(const QString& modality, bool select)
{
  Q_D(ctkModalityWidget);
  if (!d->Modalities.contains(modality))
  {
    qCritical() << Q_FUNC_INFO << "failed: unknown modality:" << modality;
    return;
  }
  QCheckBox* modalityBox = d->Modalities[modality];
  if (select == modalityBox->isChecked())
  {
    return;
  }
#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
  QSignalBlocker blocker(modalityBox);
#else
  bool wasBlocked = modalityBox->blockSignals(true);
#endif
  modalityBox->setChecked(select);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
  modalityBox->blockSignals(wasBlocked);
#endif
  d->updateAnyCheckBoxState();
  emit this->selectedModalitiesChanged(this->selectedModalities());
}

// --------------------------------------------------------------------------
void ctkModalityWidget::showModality(const QString& modality, bool show)
{
  Q_D(ctkModalityWidget);
  if (!d->Modalities.contains(modality))
  {
    qCritical() << Q_FUNC_INFO << "failed: unknown modality:" << modality;
    return;
  }
  QCheckBox* modalityBox = d->Modalities[modality];
  if (show == this->isModalitySelected(modality))
  {
    return;
  }
  modalityBox->setVisible(show);
}

// --------------------------------------------------------------------------
void ctkModalityWidget::selectAll()
{
  Q_D(ctkModalityWidget);
  bool modified = false;
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    if (!modalityBox->isChecked())
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
      QSignalBlocker blocker(modalityBox);
#else
      bool wasBlocked = modalityBox->blockSignals(true);
#endif
      modalityBox->setChecked(true);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
      modalityBox->blockSignals(wasBlocked);
#endif
      modified = true;
    }
  }
  if (modified)
  {
    d->updateAnyCheckBoxState();
    emit this->selectedModalitiesChanged(d->Modalities.keys());
  }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::unselectAll()
{
  Q_D(ctkModalityWidget);
  bool modified = false;
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    if (modalityBox->isChecked())
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
      QSignalBlocker blocker(modalityBox);
#else
      bool wasBlocked = modalityBox->blockSignals(true);
#endif
      modalityBox->setChecked(false);
#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
      modalityBox->blockSignals(wasBlocked);
#endif
      modified = true;
    }
  }
  if (modified)
  {
    d->updateAnyCheckBoxState();
    emit this->selectedModalitiesChanged(QStringList());
  }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::showAll()
{
  Q_D(ctkModalityWidget);
  foreach(const QString& modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    modalityBox->setVisible(true);
  }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::hideAll()
{
  Q_D(ctkModalityWidget);
  foreach(const QString& modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    modalityBox->setVisible(false);
  }
}

// --------------------------------------------------------------------------
bool ctkModalityWidget::areAllModalitiesSelected()const
{
  Q_D(const ctkModalityWidget);
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    if (!modalityBox->isChecked())
    {
      return false;
    }
  }
  return true;
}

// --------------------------------------------------------------------------
bool ctkModalityWidget::areAllModalitiesVisible()const
{
  Q_D(const ctkModalityWidget);
  foreach(const QString & modality, d->Modalities.keys())
  {
    QCheckBox* modalityBox = d->Modalities[modality];
    // isHidden() means explicitly hidden (not the same as !isVisible())
    if (modalityBox->isHidden())
    {
      return false;
    }
  }
  return true;
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
  Q_D(ctkModalityWidget);
  if (state == Qt::Unchecked)
  {
    this->unselectAll();
  }
  else // checked or tri-state
  {
    this->selectAll();
  }
}

// --------------------------------------------------------------------------
void ctkModalityWidget::onModalityChecked(bool checked)
{
  Q_UNUSED(checked);
  Q_D(ctkModalityWidget);
  QCheckBox* modalityBox = qobject_cast<QCheckBox*>(this->sender());
  QString modality = d->modalityFromCheckBox(modalityBox);
  d->updateAnyCheckBoxState();
  emit this->selectedModalitiesChanged(this->selectedModalities());
}

// --------------------------------------------------------------------------
bool ctkModalityWidget::isModalitySelected(const QString& modality)
{
  Q_D(ctkModalityWidget);
  if (!d->Modalities.contains(modality))
  {
    qCritical() << Q_FUNC_INFO << "failed: unknown modality:" << modality;
    return false;
  }
  QCheckBox* modalityBox = d->Modalities[modality];
  return modalityBox->isChecked();
}
