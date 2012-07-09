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

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QBuffer>
#include <QWidget>

#include <QtXmlPatterns/QXmlQuery>
#include <QtUiTools/QUiLoader>

// CTK includes
#include <ctkCommandLineParser.h>
#include <ctkCmdLineModuleXmlValidator.h>

#include "ctkCLModuleExplorerMainWindow.h"

int main(int argc, char** argv)
{
  QApplication myApp(argc, argv);

  ctkCommandLineParser cmdLineParser;
  cmdLineParser.setArgumentPrefix("--", "-");
  cmdLineParser.setStrictModeEnabled(true);

  cmdLineParser.addArgument("module", "", QVariant::String, "Path to a CLI module (executable)");
  cmdLineParser.addArgument("module-xml", "", QVariant::String, "Path to a CLI XML description.");

  cmdLineParser.addArgument("validate-module", "", QVariant::String, "Path to a CLI module");
  cmdLineParser.addArgument("validate-xml", "", QVariant::String, "Path to a CLI XML description.");
  cmdLineParser.addArgument("verbose", "v", QVariant::Bool, "Be verbose.");
  cmdLineParser.addArgument("help", "h", QVariant::Bool, "Print this help text.");

  bool parseOkay = false;
  QHash<QString, QVariant> args = cmdLineParser.parseArguments(argc, argv, &parseOkay);

  QTextStream out(stdout, QIODevice::WriteOnly);

  if(!parseOkay)
  {
    out << "Error parsing command line arguments: " << cmdLineParser.errorString() << '\n';
    return EXIT_FAILURE;
  }

  if (args.contains("help"))
  {
    out << "Usage:\n" << cmdLineParser.helpText();
    out.flush();
    return EXIT_SUCCESS;
  }

  if (args.contains("validate-module"))
  {
    if (args.contains("validate-xml"))
    {
      out << "Ignoring \"validate-xml\" option.\n\n";
    }

    QString input = args["validate-module"].toString();
    if (!QFile::exists(input))
    {
      qCritical() << "Module does not exist:" << input;
      return EXIT_FAILURE;
    }

    QProcess process;
    process.setReadChannel(QProcess::StandardOutput);
    process.start(input, QStringList("--xml"));

    if (!process.waitForFinished() || process.exitStatus() == QProcess::CrashExit ||
        process.error() != QProcess::UnknownError)
    {
      qWarning() << "The executable at" << input << "could not be started:" << process.errorString();
      return EXIT_FAILURE;
    }

    process.waitForReadyRead();
    QByteArray xml = process.readAllStandardOutput();

    if (args.contains("verbose"))
    {
      qDebug() << xml;
    }

    // validate the outputted xml description
    QBuffer xmlInput(&xml);
    xmlInput.open(QIODevice::ReadOnly);

    ctkCmdLineModuleXmlValidator validator(&xmlInput);
    if (!validator.validate())
    {
      qCritical() << validator.errorString();
      return EXIT_FAILURE;
    }

    if (args.contains("verbose"))
    {
      qDebug() << "=================================================";
      qDebug() << "****          Transformed input              ****";
      qDebug() << "=================================================";
      qDebug() << validator.output();
    }

    return EXIT_SUCCESS;
  }
  else if (args.contains("validate-xml"))
  {
    QFile input(args["validate-xml"].toString());
    if (!input.exists())
    {
      qCritical() << "XML description does not exist:" << input.fileName();
      return EXIT_FAILURE;
    }
    input.open(QIODevice::ReadOnly);

    ctkCmdLineModuleXmlValidator validator(&input);
    if (!validator.validate())
    {
      qCritical() << validator.errorString();
      return EXIT_FAILURE;
    }

    if (args.contains("verbose"))
    {
      qDebug() << "=================================================";
      qDebug() << "****          Transformed input              ****";
      qDebug() << "=================================================";
      qDebug() << validator.output();
    }
    return EXIT_SUCCESS;
  }


  //ctkCmdLineModuleDescription* descr = ctkCmdLineModuleDescription::parse(&input);

  ctkCLModuleExplorerMainWindow mainWindow;

  if (args.contains("module-xml"))
  {
    QFile input(args["module-xml"].toString());
    if (!input.exists())
    {
      qCritical() << "XML description does not exist:" << input.fileName();
      return EXIT_FAILURE;
    }
    input.open(QIODevice::ReadOnly);
    QByteArray xml = input.readAll();

    mainWindow.testModuleXML(xml);
  }
  else if (args.contains("module"))
  {
    mainWindow.addModule(args["module"].toString());
  }

  mainWindow.show();

  return myApp.exec();
}
