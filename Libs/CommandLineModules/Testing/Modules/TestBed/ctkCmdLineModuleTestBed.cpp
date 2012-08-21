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

#ifdef Q_OS_WIN
#include <windows.h>
#else
#include <time.h>
#endif

void sleep_ms(int ms)
{
#ifdef Q_OS_WIN
  Sleep(ms);
#else
  struct timespec nanostep;
  nanostep.tv_sec = static_cast<time_t>(ms / 1000);
  nanostep.tv_nsec = ((ms % 1000) * 1000.0 * 1000.0);
  nanosleep(&nanostep, NULL);
#endif
}

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

  float stepTime = outputs.size() ? runtime / static_cast<float>(outputs.size()) : runtime;

  QTime time;
  time.start();

  if (!outputs.empty())
  {
    out << "<filter-start>\n";
    out << "<filter-name>Test Filter</filter-name>\n";
    out << "<filter-comment>Does nothing useful</filter-comment>\n";
    out << "</filter-start>\n";
  }
  else
  {
    outputs.push_back("dummy");
  }

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
    sleep_ms(stepTime*1000);

    // print the first output
    if (output != "dummy")
    {
      out << output; endl(out);
      // report progress
      out << "<filter-progress>" << (i+1)*progressStep << "</filter-progress>\n";
    }
  }

  // sleep 1 second to avoid squashing the last progress event with the finished event
  sleep_ms(1000);

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
