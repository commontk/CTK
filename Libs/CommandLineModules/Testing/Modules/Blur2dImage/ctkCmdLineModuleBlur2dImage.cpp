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

#include <ctkCommandLineParser.h>

#include <QCoreApplication>
#include <QTextStream>
#include <QFile>
#include <QDebug>

#include <cstdlib>

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  // This is used by QSettings
  QCoreApplication::setOrganizationName("CommonTK");
  QCoreApplication::setApplicationName("CmdLineModuleBlur2dImage");

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");

  // Add command line argument names
  parser.addArgument("help", "h", QVariant::Bool, "Show this help text");
  parser.addArgument("xml", "", QVariant::Bool, "Print a XML description of this modules command line interface");

  QTextStream out(stdout, QIODevice::WriteOnly);
  QTextStream err(stderr, QIODevice::WriteOnly);

  // Parse the command line arguments
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(QCoreApplication::arguments(), &ok);
  if (!ok)
  {
    err << "Error parsing arguments: " << parser.errorString() << "\n";
    return EXIT_FAILURE;
  }

  // Show a help message
  if (parsedArgs.contains("help") || parsedArgs.contains("h"))
  {
    out << parser.helpText();
    return EXIT_SUCCESS;
  }

  if (parsedArgs.contains("xml"))
  {
    QFile xmlDescription(":/ctkCmdLineModuleBlur2dImage.xml");
    xmlDescription.open(QIODevice::ReadOnly);
    out << xmlDescription.readAll();
    return EXIT_SUCCESS;
  }

  // Do something

  qDebug() << "Got parameter: " << QCoreApplication::arguments();

  return EXIT_SUCCESS;
}
