
// Qt includes
#include <QApplication>
#include <QTextStream>

// CTK includes
#include <ctkPythonShell.h>
#include <ctkCommandLineParser.h>

#include "ctkSimplePythonManager.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");

  // Add command line argument names
  parser.addArgument("help", "h", QVariant::Bool, "Print usage information and exit.");
  parser.addArgument("interactive", "I", QVariant::Bool, "Enable interactive mode");

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
    QTextStream(stdout, QIODevice::WriteOnly) << "ctkSimplePythonShell\n"
          << "Usage\n\n"
          << "  ctkSimplePythonShell [options] [<path-to-python-script> ...]\n\n"
          << "Options\n"
          << parser.helpText();
    return EXIT_SUCCESS;
  }
  
  ctkSimplePythonManager pythonManager;
  
  ctkPythonShell shell(&pythonManager);
  shell.setAttribute(Qt::WA_QuitOnClose, true);
  shell.resize(600, 280);
  shell.show();

  shell.setProperty("isInteractive", parsedArgs.contains("interactive"));

  pythonManager.addObjectToPythonMain("_ctkPythonShellInstance", &shell);

  foreach(const QString& script, parser.unparsedArguments())
    {
    pythonManager.executeFile(script);
    }
  
  return app.exec();
}
