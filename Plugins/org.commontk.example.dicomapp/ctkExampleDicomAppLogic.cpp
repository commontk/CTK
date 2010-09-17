/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/


#include "ctkExampleDicomAppLogic_p.h"
#include <QtPlugin>
#include <QRect>
#include <QDebug>
#include <QPushButton>

ctkExampleDicomAppLogic::ctkExampleDicomAppLogic(ServiceAccessor<ctkDicomHostInterface> host)
  : host(host)
{
  connect(this, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)), Qt::QueuedConnection);
  emit stateChanged(ctkDicomWG23::IDLE);
}

ctkExampleDicomAppLogic::~ctkExampleDicomAppLogic()
{
}

ctkDicomWG23::State ctkExampleDicomAppLogic::getState()
{
  return ctkDicomWG23::IDLE;
}

bool ctkExampleDicomAppLogic::setState(ctkDicomWG23::State newState)
{
  qDebug() << "setState called";
  emit stateChanged(newState);
  return true;
}

bool ctkExampleDicomAppLogic::bringToFront(const QRect& /*requestedScreenArea*/)
{
  return false;
}

void ctkExampleDicomAppLogic::do_something()
{
  QPushButton *button = new QPushButton("Button from App");
  try
  {
    QRect preferred(50,50,100,100);
    qDebug() << "  Asking:getAvailableScreen";
    QRect rect = host->getAvailableScreen(preferred);
    qDebug() << "  got sth:" << rect.top();
    button->move(rect.topLeft());
    button->resize(rect.size());
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
  }
  button->show();
}

void ctkExampleDicomAppLogic::changeState(int anewstate)
{
  ctkDicomWG23::State newstate = static_cast<ctkDicomWG23::State>(anewstate);
  try
  {
    host->notifyStateChanged(newstate);
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
  }

  if (newstate == ctkDicomWG23::INPROGRESS)
  {
    do_something();
  }
}
