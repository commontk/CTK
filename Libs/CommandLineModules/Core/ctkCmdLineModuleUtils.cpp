/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
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

#include "ctkCmdLineModuleUtils.h"
#include "ctkCmdLineModuleRunException.h"

#include <QApplication>
#include <QMessageBox>
#include <QUrl>
#include <QString>

void ctkCmdLineModuleUtils::messageBoxModuleRegistration(const QFuture<ctkCmdLineModuleReferenceResult> &moduleRefsFuture,
                                                                  ctkCmdLineModuleManager::ValidationMode validationMode)
{
  QString errorMsg;
  QFutureIterator<ctkCmdLineModuleReferenceResult> futureIter(moduleRefsFuture);
  while(futureIter.hasNext())
  {
    try
    {
      const ctkCmdLineModuleReferenceResult& moduleRefResult = futureIter.next();
      if (!moduleRefResult.m_Reference)
      {
        errorMsg += (QObject::tr("Failed to register module:\n%1\n\ndue to:\n%2\n\n").arg(moduleRefResult.m_Url.toString()).arg(moduleRefResult.m_RuntimeError));
      }
      else if (!moduleRefResult.m_Reference.xmlValidationErrorString().isEmpty() &&
             validationMode == ctkCmdLineModuleManager::STRICT_VALIDATION)
      {
        errorMsg += (QObject::tr("Failed to register module:\n%1\n\ndue to xml validation error:\n%2\n\n").arg(moduleRefResult.m_Url.toString()).arg(moduleRefResult.m_Reference.xmlValidationErrorString()));
      }
    }
    // These exceptions should never happen, as at this point we are iterating over a fixed list of results, and not processing exceptions.
    catch (const ctkCmdLineModuleRunException& e)
    {
      errorMsg += QObject::tr("Failed to register module:\n") + e.location().toString() + "\n\ndue to:\n" + e.message() + "\n\n";
    }
    catch (const std::exception& e)
    {
      errorMsg += QObject::tr("Failed to register module:\n\n\ndue to:\n") + e.what() + "\n\n";
    }
  }

  if (!errorMsg.isEmpty())
  {
    QWidget* widget = QApplication::activeModalWidget();
    if (widget == NULL) widget = QApplication::activeWindow();
    QMessageBox::critical(widget, QObject::tr("Failed to register modules"), errorMsg);
  }
}
