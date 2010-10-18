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

// QT includes
#include <QDebug>
#include <QDialogButtonBox>
#include <QEvent>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

// CTK includes
#include "ctkFileDialog.h"

//------------------------------------------------------------------------------
class ctkFileDialogPrivate
{
  Q_DECLARE_PUBLIC(ctkFileDialog);
protected:
  ctkFileDialog* const q_ptr;
public:
  ctkFileDialogPrivate(ctkFileDialog& object);
  void init();
  QPushButton* acceptButton()const;
  bool AcceptButtonEnable;
  bool AcceptButtonState;
  bool IgnoreEvent;
};

//------------------------------------------------------------------------------
ctkFileDialogPrivate::ctkFileDialogPrivate(ctkFileDialog& object)
  :q_ptr(&object)
{
  this->IgnoreEvent = false;
  this->AcceptButtonEnable = true;
  this->AcceptButtonState = true;
}

//------------------------------------------------------------------------------
void ctkFileDialogPrivate::init()
{
  Q_Q(ctkFileDialog);
  QPushButton* button = this->acceptButton();
  Q_ASSERT(button);
  this->AcceptButtonState =
    button->isEnabledTo(qobject_cast<QWidget*>(button->parent()));
  // TODO: catching the event of the enable state is not enough, if the user 
  // double click on the file, the dialog will be accepted, that event should
  // be intercepted as well
  button->installEventFilter(q);
}

//------------------------------------------------------------------------------
QPushButton* ctkFileDialogPrivate::acceptButton()const
{
  Q_Q(const ctkFileDialog);
  QDialogButtonBox* buttonBox = q->findChild<QDialogButtonBox*>();
  Q_ASSERT(buttonBox);
  QDialogButtonBox::StandardButton button =
    (q->acceptMode() == QFileDialog::AcceptOpen ? QDialogButtonBox::Open : QDialogButtonBox::Save);
  return buttonBox->button(button);
}

//------------------------------------------------------------------------------
ctkFileDialog::ctkFileDialog(QWidget *parentWidget,
              const QString &caption,
              const QString &directory,
              const QString &filter)
  :QFileDialog(parentWidget, caption, directory, filter)
  , d_ptr(new ctkFileDialogPrivate(*this))
{
  Q_D(ctkFileDialog);
  d->init();
}

//------------------------------------------------------------------------------
ctkFileDialog::~ctkFileDialog()
{
}

//------------------------------------------------------------------------------
void ctkFileDialog::setBottomWidget(QWidget* widget, const QString& label)
{
  QGridLayout* gridLayout = qobject_cast<QGridLayout*>(this->layout());
  QWidget* oldBottomWidget = this->bottomWidget();
  // remove the old widget from the layout if any
  if (oldBottomWidget)
    {
    if (oldBottomWidget == widget)
      {
      return;
      }
    gridLayout->removeWidget(oldBottomWidget);
    delete oldBottomWidget;
    }
  if (widget == 0)
    {
    return;
    }
  if (!label.isEmpty())
    {
    gridLayout->addWidget(new QLabel(label), 4, 0);
    gridLayout->addWidget(widget,4, 1,1, 1);
    }
  else
    {
    gridLayout->addWidget(widget,4, 0,1, 2);
    }
  // The dialog button box is no longer spanned on 2 rows but on 3 rows if
  // there is a "bottom widget" 
  QDialogButtonBox* buttonBox = this->findChild<QDialogButtonBox*>();
  Q_ASSERT(buttonBox);
  gridLayout->removeWidget(buttonBox);
  gridLayout->addWidget(buttonBox, 2, 2, widget ? 3 : 2, 1);
}

//------------------------------------------------------------------------------
QWidget* ctkFileDialog::bottomWidget()const
{
  QGridLayout* gridLayout = qobject_cast<QGridLayout*>(this->layout());
  QLayoutItem* item = gridLayout->itemAtPosition(4,1);
  return item ? item->widget() : 0;
}

//------------------------------------------------------------------------------
void ctkFileDialog::setAcceptButtonEnable(bool enable)
{
  Q_D(ctkFileDialog);
  d->AcceptButtonEnable = enable;
  d->acceptButton()->setEnabled(d->AcceptButtonEnable && d->AcceptButtonState);
}

//------------------------------------------------------------------------------
bool ctkFileDialog::eventFilter(QObject *obj, QEvent *event)
{
  Q_D(ctkFileDialog);
  QPushButton* button = d->acceptButton();
  if (obj == button && event->type() == QEvent::EnabledChange &&
      !d->IgnoreEvent)
    {
    d->IgnoreEvent = true;
    d->AcceptButtonState = button->isEnabledTo(qobject_cast<QWidget*>(button->parent()));
    button->setEnabled(d->AcceptButtonEnable && d->AcceptButtonState);
    d->IgnoreEvent = false;
    }
  return QFileDialog::eventFilter(obj, event);
}
