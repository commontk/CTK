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
#include <QPainter>

// CTK includes
#include "ctkVTKScalarsToColorsComboBox.h"
#include "ctkVTKWidgetsUtils.h"

// VTK includes
#include <vtkScalarsToColors.h>

//-----------------------------------------------------------------------------
class ctkVTKScalarsToColorsComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKScalarsToColorsComboBox);
protected:
  ctkVTKScalarsToColorsComboBox* const q_ptr;
public:
  ctkVTKScalarsToColorsComboBoxPrivate(ctkVTKScalarsToColorsComboBox& object);
  void init();

};

// --------------------------------------------------------------------------
// ctkVTKScalarsToColorsComboBoxPrivate methods

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBoxPrivate::ctkVTKScalarsToColorsComboBoxPrivate(
  ctkVTKScalarsToColorsComboBox& object)
  : q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBoxPrivate::init()
{
  Q_Q(ctkVTKScalarsToColorsComboBox);

  QObject::connect(q->model(),
    SIGNAL(rowsAboutToBeRemoved(const QModelIndex&, int, int)),
    q, SLOT(onRowsAboutToBeRemoved(const QModelIndex&, int, int)));

  q->setIconSize(QSize(100, 20));

  // Add default raw
  q->setDefaultText(q->tr("Select a color transfer function..."));
  q->forceDefault(true);

  // Connect signals and slots
  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
    q, SLOT(onCurrentIndexChanged(int)));
}

// --------------------------------------------------------------------------
// ctkVTKScalarsToColorsComboBox methods

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBox::ctkVTKScalarsToColorsComboBox(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkVTKScalarsToColorsComboBoxPrivate(*this))
{
  Q_D(ctkVTKScalarsToColorsComboBox);
  d->init();
}

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBox::~ctkVTKScalarsToColorsComboBox()
{
  this->clear();
}

// --------------------------------------------------------------------------
int ctkVTKScalarsToColorsComboBox::addScalarsToColors(
  vtkScalarsToColors* scFunction, const QString& name)
{
  QImage img;
  if (scFunction != CTK_NULLPTR)
  {
    scFunction->Register(CTK_NULLPTR);
    img = ctk::scalarsToColorsImage(scFunction, this->iconSize());
  }
  else
  {
    img = QImage(this->iconSize(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
  }

  this->addItem(QPixmap::fromImage(img), name,
    QVariant::fromValue<void*>(scFunction));
  return count() - 1;
}

// --------------------------------------------------------------------------
vtkScalarsToColors* ctkVTKScalarsToColorsComboBox::getScalarsToColors(
  int index) const
{
  QVariant data = itemData(index);
  if (!data.isValid())
  {
    return CTK_NULLPTR;
  }

  vtkScalarsToColors* ctf =
    reinterpret_cast<vtkScalarsToColors*>(data.value<void*>());

  return ctf;
}

// --------------------------------------------------------------------------
int ctkVTKScalarsToColorsComboBox::findScalarsToColors(
  vtkScalarsToColors* scFunction) const
{
  return findData(QVariant::fromValue<void*>(scFunction));
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBox::removeScalarsToColors(
  vtkScalarsToColors* scFunction)
{
  QComboBox::removeItem(findScalarsToColors(scFunction));
}

// --------------------------------------------------------------------------
vtkScalarsToColors*
ctkVTKScalarsToColorsComboBox::currentScalarsToColors() const
{
  return getScalarsToColors(currentIndex());
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBox::setCurrentScalarsToColors(
  vtkScalarsToColors* scFunction)
{
  setCurrentIndex(findScalarsToColors(scFunction));
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBox::onCurrentIndexChanged(int index)
{
  Q_D(ctkVTKScalarsToColorsComboBox);
  emit currentScalarsToColorsChanged(getScalarsToColors(index));
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBox::onRowsAboutToBeRemoved(
  const QModelIndex&, int first, int last)
{
  for (int i = first; i <= last; ++i)
  {
    vtkScalarsToColors* scFunction = this->getScalarsToColors(i);
    if (scFunction != CTK_NULLPTR)
    {
      scFunction->Delete();
    }
  }
}
