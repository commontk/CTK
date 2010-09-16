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
  try
  {
    host->notifyStateChanged(ctkDicomWG23::IDLE);
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
  }
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
  if (newState == ctkDicomWG23::INPROGRESS)
  {
    QPushButton *button = new QPushButton("Button from App");
    button->show();
  }
  return true;
}

bool ctkExampleDicomAppLogic::bringToFront(const QRect& requestedScreenArea)
{
  return false;
}

void ctkExampleDicomAppLogic::do_something()
{
  QRect preferred;
  try
  {
    QRect rect = host->getAvailableScreen(preferred);
  }
  catch (const std::runtime_error& e)
  {
    qCritical() << e.what();
  }
}