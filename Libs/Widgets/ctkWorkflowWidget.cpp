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
#include <QApplication>
#include <QDebug>
#include <QPointer>
#include <QStyle>

// CTK includes
#include "ctkPushButton.h"
#include "ctkWorkflowWidget.h"
#include "ctkWorkflowStep.h"
#include "ctkWorkflowWidgetStep.h"
#include "ctkWorkflow.h"
#include "ctkWorkflowButtonBoxWidget.h"
#include "ctkWorkflowGroupBox.h"

// STD includes
#include <iostream>

//-----------------------------------------------------------------------------
class ctkWorkflowWidgetPrivate
{
public:
  ctkWorkflowWidgetPrivate();
  ~ctkWorkflowWidgetPrivate();

  QPointer<ctkWorkflow> Workflow;

  ctkWorkflowGroupBox*        WorkflowGroupBox;
  ctkWorkflowButtonBoxWidget* ButtonBoxWidget;

  bool ShowButtonBoxWidget;
};

// --------------------------------------------------------------------------
// ctkWorkflowWidgetPrivate methods

//---------------------------------------------------------------------------
ctkWorkflowWidgetPrivate::ctkWorkflowWidgetPrivate()
{
  this->WorkflowGroupBox = 0;
  this->ButtonBoxWidget = 0;

  this->ShowButtonBoxWidget = true;
}

//---------------------------------------------------------------------------
ctkWorkflowWidgetPrivate::~ctkWorkflowWidgetPrivate()
{
  if (!this->Workflow.isNull())
    {
    foreach(ctkWorkflowStep* step, this->Workflow.data()->steps())
      {
      ctkWorkflowWidgetStep * widgetStep = dynamic_cast<ctkWorkflowWidgetStep*>(step);
      if (widgetStep)
        {
        widgetStep->setVisible(false);
        widgetStep->setParent(0);
        }
      }
    }
}

// --------------------------------------------------------------------------
// ctkWorkflowWidgetMethods

// --------------------------------------------------------------------------
ctkWorkflowWidget::ctkWorkflowWidget(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkWorkflowWidgetPrivate)
{
  Q_D(ctkWorkflowWidget);
  d->WorkflowGroupBox = new ctkWorkflowGroupBox(this);
  d->ButtonBoxWidget = new ctkWorkflowButtonBoxWidget();
}

// --------------------------------------------------------------------------
ctkWorkflowWidget::~ctkWorkflowWidget()
{
}

// --------------------------------------------------------------------------
ctkWorkflow* ctkWorkflowWidget::workflow()const
{
  Q_D(const ctkWorkflowWidget);
  return d->Workflow.data();
}

// --------------------------------------------------------------------------
CTK_GET_CPP(ctkWorkflowWidget, ctkWorkflowGroupBox*, workflowGroupBox, WorkflowGroupBox);
CTK_GET_CPP(ctkWorkflowWidget, bool, showButtonBoxWidget, ShowButtonBoxWidget);
CTK_SET_CPP(ctkWorkflowWidget, bool, setShowButtonBoxWidget, ShowButtonBoxWidget);
CTK_GET_CPP(ctkWorkflowWidget, ctkWorkflowButtonBoxWidget*, buttonBoxWidget, ButtonBoxWidget);

// --------------------------------------------------------------------------
void ctkWorkflowWidget::setWorkflow(ctkWorkflow* newWorkflow)
{
  Q_D(ctkWorkflowWidget);

  if (!newWorkflow)
    {
    qWarning() << "setWorkflow - cannot set workflow to NULL";
    return;
    }

  if (!d->Workflow.isNull())
    {
    QObject::disconnect(d->Workflow.data(), SIGNAL(currentStepChanged(ctkWorkflowStep*)),
                        this, SLOT(onCurrentStepChanged(ctkWorkflowStep)));
    QObject::disconnect(d->Workflow.data(), SIGNAL(stepRegistered(ctkWorkflowStep*)),
                        this, SLOT(onStepRegistered(ctkWorkflowStep)));
    }

  d->Workflow = newWorkflow;

  if (!d->Workflow.isNull())
    {
    foreach(ctkWorkflowStep* step, d->Workflow.data()->steps())
      {
      this->onStepRegistered(step);
      }
    }

  QObject::connect(newWorkflow, SIGNAL(currentStepChanged(ctkWorkflowStep*)), this, SLOT(onCurrentStepChanged(ctkWorkflowStep*)));
  QObject::connect(newWorkflow, SIGNAL(stepRegistered(ctkWorkflowStep*)), this, SLOT(onStepRegistered(ctkWorkflowStep*)));

  d->ButtonBoxWidget->setWorkflow(newWorkflow);
}

// --------------------------------------------------------------------------
ctkWorkflowWidgetStep* ctkWorkflowWidget::widgetStep(const QString& id)const
{
  Q_D(const ctkWorkflowWidget);
  return dynamic_cast<ctkWorkflowWidgetStep*>(
    !d->Workflow.isNull() ? d->Workflow.data()->step(id) : 0);
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::onCurrentStepChanged(ctkWorkflowStep* currentStep)
{
  if (currentStep)
    {
    this->updateStepUI(currentStep);
    this->updateButtonBoxUI(currentStep);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::onStepRegistered(ctkWorkflowStep* step)
{
  if (step->isWidgetType())
    {
    QWidget * widget = dynamic_cast<QWidget*>(step);
    Q_ASSERT(widget);
    widget->setParent(this);
    widget->setVisible(false);
    }
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::updateStepUI(ctkWorkflowStep* currentStep)
{
  Q_D(ctkWorkflowWidget);
  Q_ASSERT(currentStep);
  Q_ASSERT(d->WorkflowGroupBox);

  // Create layout and WorkflowGroupBox if this is our first time here
  if (!this->layout())
    {
    QVBoxLayout* layout = new QVBoxLayout();
    this->setLayout(layout);
    layout->addWidget(d->WorkflowGroupBox);

    if (d->ShowButtonBoxWidget)
      {
      layout->addWidget(d->ButtonBoxWidget);
      }

    layout->setContentsMargins(0,0,0,0);
    }

  d->WorkflowGroupBox->updateGroupBox(currentStep);
}

// --------------------------------------------------------------------------
void ctkWorkflowWidget::updateButtonBoxUI(ctkWorkflowStep* currentStep)
{
  Q_D(ctkWorkflowWidget);
  Q_ASSERT(currentStep);

  // Update the button box widget if we want to show it
  if (d->ShowButtonBoxWidget)
    {
    d->ButtonBoxWidget->updateButtons(currentStep);
    }
}

//-----------------------------------------------------------------------------
QVariant ctkWorkflowWidget::buttonItem(QString item,
                                       ctkWorkflowWidgetStep* step)
{
  QRegExp backRegExp("^[\\{\\(\\[]back:(.*)[\\}\\)\\]]$");
  QRegExp nextRegExp("^[\\{\\(\\[]next:(.*)[\\}\\)\\]]$");
  QRegExp currentRegExp("^[\\{\\(\\[]current:(.*)[\\}\\)\\]]$");
  if (backRegExp.exactMatch(item))
    {
    QList<ctkWorkflowStep*> backs =
      (step ? step->workflow()->backwardSteps(step) : QList<ctkWorkflowStep*>());
    step = (backs.size() ? dynamic_cast<ctkWorkflowWidgetStep*>(backs[0]) : 0);
    item.remove("back:");
    return ctkWorkflowWidget::buttonItem(item, step);
    }
  else if (nextRegExp.exactMatch(item))
    {
    QList<ctkWorkflowStep*> nexts =
      step ? step->workflow()->forwardSteps(step) : QList<ctkWorkflowStep*>();
    step = (nexts.size() ? dynamic_cast<ctkWorkflowWidgetStep*>(nexts[0]) : 0);
    item.remove("next:");
    return ctkWorkflowWidget::buttonItem(item, step);
    }
  else if (currentRegExp.exactMatch(item))
    {
    item.remove("current:");
    }
  QVariant res;
  QRegExp quotesRegExp("^\"(.*)\"$");
  QRegExp propsRegExp("^[\\{\\(\\[](.*)[\\}\\)\\]]$");
  QStyle* style = (step ? step->style() : qApp->style());
  if (item == "[<-]")
    {
    res.setValue(style->standardIcon(QStyle::SP_ArrowLeft));
    }
  else if (item == "[->]")
    {
    res.setValue(style->standardIcon(QStyle::SP_ArrowRight));
    }
  else if (item == "{#}" || item == "(#)")
    {
    res = QVariant(step ? step->workflow()->backwardDistanceToStep(step) + 1 : 0);
    }
  else if (item == "{!#}" || item == "(!#)")
    {
    res = QVariant(step ? step->workflow()->steps().count() : 0);
    }
  else if (quotesRegExp.exactMatch(item))
    {
    res = quotesRegExp.cap(1);
    }
  else if (propsRegExp.exactMatch(item))
    {
    item = propsRegExp.cap(1);
    if (quotesRegExp.exactMatch(item))
      {
      res = quotesRegExp.cap(1);
      }
    else
      {
      res = step ? step->property(item.toLatin1()) : QVariant();
      if (res.isValid() && res.type() == QVariant::String && res.toString().isEmpty())
        {
        res = QVariant();
        }
      }
    }
  else
    {
    qWarning() << "Item" << item << "not supported";
    }
  return res;
}

//-----------------------------------------------------------------------------
void ctkWorkflowWidget
::formatButton(QAbstractButton* button, const QString& buttonFormat,
               ctkWorkflowWidgetStep* step)
{
  QMap<QString, QVariant> formats =
    ctkWorkflowWidget::parse(buttonFormat, step);
  button->setIcon(formats["icon"].value<QIcon>());
  if (qobject_cast<ctkPushButton*>(button))
    {
    qobject_cast<ctkPushButton*>(button)->setIconAlignment(
      static_cast<Qt::Alignment>(formats["iconalignment"].toInt()));
    }
  button->setText(formats["text"].toString());
  button->setToolTip(formats["tooltip"].toString());
}

//-----------------------------------------------------------------------------
QString ctkWorkflowWidget
::formatText(const QString& textFormat, ctkWorkflowWidgetStep* step)
{
  QMap<QString, QVariant> formats =
    ctkWorkflowWidget::parse(textFormat, step);
  return formats["text"].toString();
}

//-----------------------------------------------------------------------------
QMap<QString, QVariant> ctkWorkflowWidget
::parse(const QString& format, ctkWorkflowWidgetStep* step)
{
  QIcon buttonIcon;
  Qt::Alignment buttonIconAlignment = Qt::AlignLeft | Qt::AlignVCenter;
  QString buttonText;
  QString buttonToolTip;

  QString textRegExp("\\{[^{}]+\\}");
  QString simpleTextRegExp("\"[^\"]+\"");
  QString toolTipRegExp("\\([^\\(\\)]+\\)");
  QString iconRegExp("\\[[^\\[\\]]+\\]");

  //QRegExp splitBrackets("\\{([^}]+)\\}");
  //QRegExp splitBrackets("(\\{[^{}]+\\}|\\([^\\(\\)]+\\)|\"[^\"]+\")");
  QRegExp splitBrackets(QString("(%1|%2|%3|%4)")
                        .arg(textRegExp).arg(simpleTextRegExp)
                        .arg(toolTipRegExp)
                        .arg(iconRegExp));
  QStringList brackets;
  int pos = 0;
  while ((pos = splitBrackets.indexIn(format, pos)) != -1)
    {
    brackets << splitBrackets.cap(1);
    pos += splitBrackets.matchedLength();
    }

  foreach(const QString& withBracket, brackets)
    {
    bool isSimpleText =
      QRegExp(QString("^") + simpleTextRegExp + QString("$")).exactMatch(withBracket);

    QString withoutBracket = withBracket.mid(1, withBracket.size() - 2);
    // If the item is empty, then check the next item. For example:
    // {next:description|next:name|next:stepid}: if 'next:description' is empty, then
    // use 'next:name' if not empty, otherwise 'next:stepid'
    // Don't split simple text, it is never empty.
    QStringList tokens = isSimpleText ? QStringList(withoutBracket) : withoutBracket.split('|');

    QIcon icon;
    Qt::Alignment iconAlignment = buttonIconAlignment;
    QString text;
    foreach (const QString& token, tokens)
      {
      QString tokenWithBracket = withBracket[0] + token + withBracket[withBracket.size()-1];
      QVariant item = ctkWorkflowWidget::buttonItem(tokenWithBracket, step);
      if (item.isValid())
        {
        switch (item.type())
          {
          case QVariant::Icon:
            icon = item.value<QIcon>();
            if (!buttonText.isEmpty())
              {
              iconAlignment = Qt::AlignRight | Qt::AlignVCenter;
              }
            break;
          case QVariant::String:
          case QVariant::Int:
            text += item.toString();
            break;
          default:
            break;
          }
        // skip the other cases if the item was valid, otherwise keep on searching
        break;
        }
      }
    if (QRegExp(QString("^") + textRegExp + QString("$")).exactMatch(withBracket) ||
        isSimpleText)
      {
      buttonText += text;
      }
    else if (QRegExp(QString("^") + iconRegExp + QString("$")).exactMatch(withBracket))
      {
      buttonIcon = icon;
      buttonIconAlignment = iconAlignment;
      }
    else if (QRegExp(QString("^") + toolTipRegExp + QString("$")).exactMatch(withBracket))
      {
      buttonToolTip = text;
      }
    }
  QMap<QString, QVariant> formats;
  formats["icon"] = buttonIcon;
  formats["iconalignment"] = static_cast<int>(buttonIconAlignment);
  formats["text"] = buttonText;
  formats["tooltip"] = buttonToolTip;
  return formats;
}
