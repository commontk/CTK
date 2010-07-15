#ifndef __ctkCommandLineParser_h
#define __ctkCommandLineParser_h

// Qt includes
#include <QString>
#include <QStringList>
#include <QVariant>

// CTK includes
#include "CTKCoreExport.h"

// --------------------------------------------------------------------------
class CTK_CORE_EXPORT ctkCommandLineParser
{
public:

  ctkCommandLineParser();
  ~ctkCommandLineParser();
  
  QHash<QString /*longarg*/, QVariant> parseArguments(const QStringList &arguments, bool* ok = 0);

  QString errorString();
  
  const QStringList& unparsedArguments();
  
  bool argumentAdded(const QString& argument);

  bool argumentParsed(const QString& argument);

  void addArgument(const QString& longarg, const QString& shortarg,
                   QVariant::Type type, const QString& argHelp = QString(),
                   const QVariant& defaultValue = QVariant(), bool ignoreRest = false);

  bool setExactMatchRegularExpression(const QString& argument, const QString& expression,
                                      const QString& ExactMatchFailedMessage);

  int fieldWidth();

  QString helpText(const char charPad = ' ');
  
private:
  class ctkInternal;
  ctkInternal * Internal;
};
#endif
