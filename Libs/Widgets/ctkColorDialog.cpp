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

// QT includes
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QVariant>

// CTK includes
#include "ctkColorDialog.h"

QList<QWidget*> ctkColorDialog::DefaultTabs;
int ctkColorDialog::DefaultTab = -1;
QString ctkColorDialog::LastColorName = QString();

//------------------------------------------------------------------------------
class ctkColorDialogPrivate
{
  Q_DECLARE_PUBLIC(ctkColorDialog);
protected:
  ctkColorDialog* const q_ptr;
public:
  ctkColorDialogPrivate(ctkColorDialog& object);
  void init();
  QTabWidget* LeftTabWidget;
  QWidget*    BasicTab;
  QString ColorName;
};

//------------------------------------------------------------------------------
ctkColorDialogPrivate::ctkColorDialogPrivate(ctkColorDialog& object)
  :q_ptr(&object)
{
  this->LeftTabWidget = 0;
}

//------------------------------------------------------------------------------
void ctkColorDialogPrivate::init()
{
  Q_Q(ctkColorDialog);
  QVBoxLayout* mainLay = qobject_cast<QVBoxLayout*>(q->layout());
  QHBoxLayout* topLay = qobject_cast<QHBoxLayout*>(mainLay->itemAt(0)->layout());
  QVBoxLayout* leftLay = qobject_cast<QVBoxLayout*>(topLay->takeAt(0)->layout());
  
  leftLay->setParent(0);
  this->BasicTab = new QWidget(q);
  this->BasicTab->setLayout(leftLay);

  this->LeftTabWidget = new QTabWidget(q);
  topLay->insertWidget(0, this->LeftTabWidget);
  this->LeftTabWidget->addTab(this->BasicTab, QObject::tr("Basic"));
  
  // If you use a ctkColorDialog, it's probably because you have tabs to add
  // into. Which means that you are likely to want to resize the dialog as
  // well.
  q->setSizeGripEnabled(true);
  q->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);

  QObject::connect(q, SIGNAL(currentColorChanged(QColor)),
                   q, SLOT(resetColorName()));
}

//------------------------------------------------------------------------------
ctkColorDialog::ctkColorDialog(QWidget* parent)
  : QColorDialog(parent)
  , d_ptr(new ctkColorDialogPrivate(*this))
{
  Q_D(ctkColorDialog);
  // Force using Qt's standard color dialog to support adding new widgets
  setOption(QColorDialog::DontUseNativeDialog);
  d->init();
}

//------------------------------------------------------------------------------
ctkColorDialog::ctkColorDialog(const QColor& initial, QWidget* parent)
  : QColorDialog(initial, parent)
  , d_ptr(new ctkColorDialogPrivate(*this))
{
  Q_D(ctkColorDialog);
  // Force using Qt's standard color dialog to support adding new widgets
  setOption(QColorDialog::DontUseNativeDialog);
  d->init();
}

//------------------------------------------------------------------------------
ctkColorDialog::~ctkColorDialog()
{
}

//------------------------------------------------------------------------------
void ctkColorDialog::insertTab(int tabIndex, QWidget* widget, const QString& label)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->insertTab(tabIndex, widget, label);
}

//------------------------------------------------------------------------------
void ctkColorDialog::setCurrentTab(int index)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->setCurrentIndex(index);
}

//------------------------------------------------------------------------------
void ctkColorDialog::removeTab(int index)
{
  Q_D(ctkColorDialog);
  d->LeftTabWidget->removeTab(index);
}

//------------------------------------------------------------------------------
int ctkColorDialog::indexOf(QWidget* widget)const
{
  Q_D(const ctkColorDialog);
  return d->LeftTabWidget->indexOf(widget);
}

//------------------------------------------------------------------------------
QWidget* ctkColorDialog::widget(int index)const
{
  Q_D(const ctkColorDialog);
  return d->LeftTabWidget->widget(index);
}

//------------------------------------------------------------------------------
QColor ctkColorDialog::getColor(const QColor &initial, QWidget *parent, const QString &title,
                              ColorDialogOptions options)
{
  ctkColorDialog dlg(parent);
  if (!title.isEmpty())
    {
    dlg.setWindowTitle(title);
    }
  dlg.setOptions(options | QColorDialog::DontUseNativeDialog);
  dlg.setCurrentColor(initial);
  foreach(QWidget* tab, ctkColorDialog::DefaultTabs)
    {
    dlg.insertTab(tab->property("tabIndex").toInt(), tab, tab->windowTitle());
    if (!tab->property("colorSignal").isNull())
      {
      QObject::connect(tab, tab->property("colorSignal").toString().toLatin1(),
                       &dlg, SLOT(setColor(QColor)));
      }
    if (!tab->property("nameSignal").isNull())
      {
      QObject::connect(tab, tab->property("nameSignal").toString().toLatin1(),
                       &dlg, SLOT(setColorName(QString)));
      }
    }
  dlg.setCurrentTab(ctkColorDialog::DefaultTab);
  dlg.exec();
  foreach(QWidget* tab, ctkColorDialog::DefaultTabs)
    {
    dlg.removeTab(dlg.indexOf(tab));
    if (!tab->property("colorSignal").isNull())
      {
      QObject::disconnect(tab, tab->property("colorSignal").toString().toLatin1(),
                          &dlg, SLOT(setColor(QColor)));
      }
    if (!tab->property("nameSignal").isNull())
      {
      QObject::disconnect(tab, tab->property("nameSignal").toString().toLatin1(),
                          &dlg, SLOT(setColorName(QString)));
      }
    tab->setParent(0);
    tab->hide();
    }
  ctkColorDialog::LastColorName = dlg.colorName();
  return dlg.selectedColor();
}

//------------------------------------------------------------------------------
QString ctkColorDialog::getColorName()
{
  return ctkColorDialog::LastColorName;
}

//------------------------------------------------------------------------------
void ctkColorDialog::insertDefaultTab(int tabIndex, QWidget* widget,
                                      const QString& label,
                                      const char* colorSignal,
                                      const char* nameSignal)
{
  widget->setWindowTitle(label);
  widget->setProperty("colorSignal", colorSignal);
  widget->setProperty("nameSignal", nameSignal);
  widget->setProperty("tabIndex", tabIndex);

  ctkColorDialog::DefaultTabs << widget;
  widget->setParent(0);
}

//------------------------------------------------------------------------------
void ctkColorDialog::setDefaultTab(int index)
{
  ctkColorDialog::DefaultTab = index;
}

//------------------------------------------------------------------------------
void ctkColorDialog::setColor(const QColor& color)
{
  this->QColorDialog::setCurrentColor(color);
}

//------------------------------------------------------------------------------
void ctkColorDialog::setColorName(const QString& name)
{
  Q_D(ctkColorDialog);
  if (d->ColorName == name)
    {
    return;
    }
  d->ColorName = name;
  emit currentColorNameChanged(d->ColorName);
}

//------------------------------------------------------------------------------
QString ctkColorDialog::colorName()const
{
  Q_D(const ctkColorDialog);
  return d->ColorName;
}

//------------------------------------------------------------------------------
void ctkColorDialog::resetColorName()
{
  this->setColorName(QString());
}
