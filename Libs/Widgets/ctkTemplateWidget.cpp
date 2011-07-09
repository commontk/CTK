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

// CTK includes
#include "ctkTemplateWidget.h"
#include "ui_ctkTemplateWidget.h"
#include "ctkLogger.h"

static ctkLogger logger("org.commontk.libs.widgets.ctkTemplateWidget");

//-----------------------------------------------------------------------------
class ctkTemplateWidgetPrivate: public Ui_ctkTemplateWidget
{
  Q_DECLARE_PUBLIC(ctkTemplateWidget);
protected:
  ctkTemplateWidget* const q_ptr;
public:
  ctkTemplateWidgetPrivate(ctkTemplateWidget& object);
  void init();
};

// --------------------------------------------------------------------------
ctkTemplateWidgetPrivate::ctkTemplateWidgetPrivate(ctkTemplateWidget& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void ctkTemplateWidgetPrivate::init()
{
  Q_Q(ctkTemplateWidgetPrivate);
  this->setupUi(q);
}

// --------------------------------------------------------------------------
ctkTemplateWidget::ctkTemplateWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkTemplateWidgetPrivate(*this))
{
  Q_D(ctkTemplateWidget);
  d->init();
}

// --------------------------------------------------------------------------
ctkTemplateWidget::~ctkTemplateWidget()
{
}
