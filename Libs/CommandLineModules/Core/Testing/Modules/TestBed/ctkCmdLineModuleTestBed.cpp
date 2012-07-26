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
#include <QTime>

#include <cstdlib>
#include <time.h>

int main(int argc, char* argv[])
{
  QCoreApplication app(argc, argv);
  // This is used by QSettings
  QCoreApplication::setOrganizationName("CommonTK");
  QCoreApplication::setApplicationName("CmdLineModuleTestBed");

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");

  // Add command line argument names
  parser.addArgument("help", "h", QVariant::Bool, "Show this help text");
  parser.addArgument("xml", "", QVariant::Bool, "Print a XML description of this modules command line interface");
  parser.addArgument("runtime", "", QVariant::Int, "Runtime in seconds", 1);
  parser.addArgument("numOutputs", "", QVariant::Int, "Number of outpusts", 0);
  parser.addArgument("exitCode", "", QVariant::Int, "Exit code", 0);
  parser.addArgument("exitCrash", "", QVariant::Bool, "Force crash", false);
  parser.addArgument("exitTime", "", QVariant::Int, "Exit time", 0);
  parser.addArgument("errorText", "", QVariant::String, "Error text (will not be printed on exit code 0)");
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
    QFile xmlDescription(":/ctkCmdLineModuleTestBed.xml");
    xmlDescription.open(QIODevice::ReadOnly);
    out << xmlDescription.readAll();
    return EXIT_SUCCESS;
  }

  // Do something

  float runtime = parsedArgs["runtime"].toFloat();
  int numOutputs = parsedArgs["numOutputs"].toInt();
  float exitTime = parsedArgs["exitTime"].toFloat();
  int exitTimeMillis = static_cast<long>(exitTime/2.0 * 1000.0);
  int exitCode = parsedArgs["exitCode"].toInt();
  bool exitCrash = parsedArgs["exitCrash"].toBool();
  QString errorText = parsedArgs["errorText"].toString();

  QStringList outputs;
  for (int i = 0; i < numOutputs; ++i)
  {
    outputs << "Output " + QString::number(i+1);
  }

  if (outputs.empty())
  {
    // no outputs given, just return
    if (exitCrash)
    {
      int* crash = 0;
      *crash = 5;
    }
    if (exitCode != 0)
    {
      err << errorText;
    }
    return exitCode;
  }

  float stepTime = runtime / static_cast<float>(outputs.size());

  QTime time;
  time.start();

  struct timespec nanostep;

  out << "<filter-start>\n";
  out << "<filter-name>Test Filter</filter-name>\n";
  out << "<filter-comment>Does nothing useful</filter-comment>\n";
  out << "</filter-start>\n";

  float progressStep = 1.0f / static_cast<float>(outputs.size());
  for(int i = 0; i < outputs.size(); ++i)
  {
    QString output = outputs[i];

    if (exitTimeMillis != 0 && exitTimeMillis < time.elapsed())
    {
      if (exitCrash)
      {
        int* crash = 0;
        *crash = 5;
      }
      if (exitCode != 0 && !errorText.isEmpty())
      {
        err << errorText;
      }
      return exitCode;
    }

    // simulate some work
    nanostep.tv_sec = static_cast<time_t>(stepTime);
    double millisecs = (stepTime - static_cast<time_t>(stepTime)) * 1000.0;
    nanostep.tv_nsec = static_cast<long>(millisecs * 1000.0 * 1000.0);
    nanosleep(&nanostep, NULL);

    // print the first output
    out << output; endl(out);

    // report progress
    out << "<filter-progress>" << (i+1)*progressStep << "</filter-progress>\n";
  }

  // sleep 1 second to avoid squashing the last progress event with the finished event
  sleep(1);

  if (exitCrash)
  {
    int* crash = 0;
    *crash = 5;
  }
  if (exitCode != 0 && !errorText.isEmpty())
  {
    err << errorText;
  }
  return exitCode;
}
