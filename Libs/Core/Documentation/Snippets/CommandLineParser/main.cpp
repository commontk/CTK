//! [0]
#include <ctkCommandLineParser.h>

#include <QCoreApplication>
#include <QTextStream>

#include <cstdlib>

int main(int argc, char** argv)
{
  QCoreApplication app(argc, argv);
  // This is used by QSettings
  QCoreApplication::setOrganizationName("MyOrg");
  QCoreApplication::setApplicationName("MyApp");

  ctkCommandLineParser parser;
  // Use Unix-style argument names
  parser.setArgumentPrefix("--", "-");
  // Enable QSettings support
  parser.enableSettings("disable-settings");

  // Add command line argument names
  parser.addArgument("disable-settings", "", QVariant::Bool, "Do not use QSettings");
  parser.addArgument("help", "h", QVariant::Bool, "Show this help text");
  parser.addArgument("search-paths", "s", QVariant::StringList, "A list of paths to search");

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

  // Do something

  return EXIT_SUCCESS;
}
//! [0]
