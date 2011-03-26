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

// Qt includes
#include <QCoreApplication>
#include <QDebug>
#include <QStringList>

// CTK includes
#include "ctkErrorLogModel.h"
#include "ctkVTKErrorLogMessageHandler.h"
#include "ctkModelTester.h"

// VTK includes
#include <vtkOutputWindow.h>

// STL includes
#include <cstdlib>
#include <iostream>

namespace
{
//-----------------------------------------------------------------------------
// Utility function

//-----------------------------------------------------------------------------
QString checkRowCount(int line, int currentRowCount, int expectedRowCount)
{
  if (currentRowCount != expectedRowCount)
    {
    QString errorMsg("Line %1 - Expected rowCount: %2 - Current rowCount: %3\n");
    return errorMsg.arg(line).arg(expectedRowCount).arg(currentRowCount);
    }
  return QString();
}

//-----------------------------------------------------------------------------
QString checkTextMessages(int line, const ctkErrorLogModel& model, const QStringList& expectedMessages)
{
  for(int i=0; i < expectedMessages.count(); ++i)
    {
    QModelIndex descriptionIndex = model.index(i, ctkErrorLogModel::DescriptionColumn);
    QString currentMessage = descriptionIndex.data(ctkErrorLogModel::DescriptionTextRole).toString();
    if (currentMessage.compare(expectedMessages.value(i)) != 0)
      {
      QString errorMsg("Line %1 - Problem with row%2 !\n"
                       "\tExpected message [%3]\n"
                       "\tCurrent message [%4]\n");
      return errorMsg.arg(line).arg(i).arg(expectedMessages.value(i)).arg(currentMessage);
      }
    }
  return QString();
}

//-----------------------------------------------------------------------------
void printTextMessages(const ctkErrorLogModel& model)
{
  fprintf(stdout, "%s", "ErrorLogModel rows:\n");
  QString text("\trow %1 => %2\n");
  for (int i=0; i < model.rowCount(); ++i)
    {
    QString description =
        model.index(0, ctkErrorLogModel::DescriptionColumn).data().toString();
    fprintf(stdout, "%s", qPrintable(text.arg(i).arg(description)));
    }
  fflush(stdout);
}

//-----------------------------------------------------------------------------
void printErrorMessage(const QString& errorMessage)
{
  fprintf(stderr, "%s", qPrintable(errorMessage));
  fflush(stderr);
}

} // end namespace

//-----------------------------------------------------------------------------
int ctkVTKErrorLogModelTest1(int argc, char * argv [])
{
  QCoreApplication app(argc, argv);
  Q_UNUSED(app);

  ctkErrorLogModel model;
  ctkModelTester modelTester;
  modelTester.setVerbose(false);
  QString errorMsg;

  try
    {
    modelTester.setModel(&model);

    // --------------------------------------------------------------------------
    // Monitor VTK messages

    model.registerMsgHandler(new ctkVTKErrorLogMessageHandler);
    model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, true);

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }

    QString vtkMessage0("This is a VTK debug message");
    vtkOutputWindowDisplayDebugText(qPrintable(vtkMessage0));

    QString vtkMessage1("This is a VTK warning message");
    vtkOutputWindowDisplayWarningText(qPrintable(vtkMessage1));

    QString vtkMessage2("This is a VTK error message");
    vtkOutputWindowDisplayErrorText(qPrintable(vtkMessage2));

    QStringList expectedVTKMessages;
    expectedVTKMessages << vtkMessage0 << vtkMessage1 << vtkMessage2;

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ expectedVTKMessages.count());
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      return EXIT_FAILURE;
      }

    errorMsg = checkTextMessages(__LINE__, model, expectedVTKMessages);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      return EXIT_FAILURE;
      }

    // Clear
    model.clear();

    // Disable VTK messages monitoring
    model.setMsgHandlerEnabled(ctkVTKErrorLogMessageHandler::HandlerName, false);

    vtkOutputWindowDisplayDebugText("This VTK debug message should appear in the console");
    vtkOutputWindowDisplayWarningText("This VTK warning message should appear in the console");
    vtkOutputWindowDisplayErrorText("This VTK error message should appear in the console");

    errorMsg = checkRowCount(__LINE__, model.rowCount(), /* expected = */ 0);
    if (!errorMsg.isEmpty())
      {
      model.disableAllMsgHandler();
      printErrorMessage(errorMsg);
      printTextMessages(model);
      return EXIT_FAILURE;
      }
    }
  catch (const char* error)
    {
    model.disableAllMsgHandler();
    std::cerr << error << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
