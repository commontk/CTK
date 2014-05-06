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

#include <QApplication>
#include <QTextStream>
#include <QFile>
#include <QPixmap>
#include <QPainter>

#include <cstdlib>

int main(int argc, char* argv[])
{
  // QPixmap (used below) requires QApplication (instead of QCoreApplication)
  QApplication app(argc, argv);
  // This is used by QSettings
  QCoreApplication::setOrganizationName("CommonTK");
  QCoreApplication::setApplicationName("CmdLineModuleTour");

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");

  // Add command line argument names
  parser.addArgument("help", "h", QVariant::Bool, "Show this help text");
  parser.addArgument("xml", "", QVariant::Bool, "Print a XML description of this modules command line interface");

  parser.addArgument("integer", "i", QVariant::Int, "Show this help text",15);
  parser.addArgument("", "b", QVariant::String, "Show this help text");
  parser.addArgument("double", "d", QVariant::Double, "Show this help text");
  parser.addArgument("floatVector", "f", QVariant::String, "Show this help text");
  parser.addArgument("enumeration", "e", QVariant::String, "Show this help text");
  parser.addArgument("string_vector", "", QVariant::String, "Show this help text");
  parser.addArgument("", "p", QVariant::String, "Show this help text");

  // Parse the command line arguments
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(QCoreApplication::arguments(), &ok);
  if (!ok)
  {
    QTextStream(stderr, QIODevice::WriteOnly) << "Error parsing arguments: "
                                              << parser.errorString() << "\n";
    return EXIT_FAILURE;
  }

  // Show a help message
  if (parsedArgs.contains("help") || parsedArgs.contains("h"))
  {
    QTextStream(stdout, QIODevice::WriteOnly) << parser.helpText();
    return EXIT_SUCCESS;
  }

  if (parsedArgs.contains("xml"))
  {
    QFile xmlDescription(":/ctkCmdLineModuleTour.xml");
    xmlDescription.open(QIODevice::ReadOnly);
    QTextStream(stdout, QIODevice::WriteOnly) << xmlDescription.readAll();
  }
  if(parsedArgs.contains("p"))
  {
    QTextStream(stdout, QIODevice::WriteOnly) << parsedArgs["p"].toString();
  }
  if(parsedArgs.contains("double"))
  {
    QTextStream(stdout, QIODevice::WriteOnly) << parsedArgs["double"].toString();
  }

  // Do something
  // do we have enough information (input/output)?
  if(parser.unparsedArguments().count() >= 2) 
  {
    QString input = parser.unparsedArguments().at(0);
    QString output = parser.unparsedArguments().at(1);

    QPixmap pix(input);
    QPainter painter(&pix);
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", parsedArgs["integer"].toInt()));
    painter.drawText(pix.rect(),Qt::AlignBottom|Qt::AlignLeft,"Result image produced by ctkCLIModuleTour");
    pix.save(output, "JPEG");
  }

  return EXIT_SUCCESS;
}
