
// Qt includes
#include <QDebug>

// CTK includes
#include "ctkCommandLineParser.h"

// STD includes
#include <cstdlib>

int ctkCommandLineParserTest1(int, char*[])
{
  // Test1 - Check if unparsedArguments() worked as expected
  QStringList arguments1;
  arguments1 << "ctkCommandLineParserTest1";
  arguments1 << "--test-bool";
  arguments1 << "--test-string";
  arguments1 << "ctkrocks";
  ctkCommandLineParser parser1;
  bool ok = false;
  parser1.parseArguments(arguments1, &ok);
  if (!ok)
    {
    qCritical() << "Test1 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  QStringList expectedUnparsedArguments1;
  expectedUnparsedArguments1 << "--test-bool" << "--test-string" << "ctkrocks";

  if (parser1.unparsedArguments() != expectedUnparsedArguments1)
    {
    qCritical() << "unparsedArguments:" << parser1.unparsedArguments();
    qCritical() << "expectedUnparsedArguments1:" << expectedUnparsedArguments1;
    return EXIT_FAILURE;
    }

  // Test2 - Check if addArgument() for a boolean workes as expected
  QStringList arguments2;
  arguments2 << "ctkCommandLineParserTest1";
  arguments2 << "--test-bool";
  arguments2 << "--test-string";
  arguments2 << "ctkrocks";
  ctkCommandLineParser parser2;
  parser2.addArgument("--test-bool", "", QVariant::Bool, "This is a test bool", false);
  ok = false;
  QHash<QString, QVariant> parsedArgs = parser2.parseArguments(arguments2, &ok);
  if (!ok)
    {
    qCritical() << "Test2 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  QStringList expectedUnparsedArguments2;
  expectedUnparsedArguments2 << "--test-string" << "ctkrocks";

  if (parser2.unparsedArguments() != expectedUnparsedArguments2)
    {
    qCritical() << "Test2 - Failed - unparsedArguments:" << parser2.unparsedArguments()
        << ", expectedUnparsedArguments2:" << expectedUnparsedArguments2;
    return EXIT_FAILURE;
    }

  if (parsedArgs["--test-bool"].isNull() || !parsedArgs["--test-bool"].toBool())
    {
    qCritical() << "Test2 - Failed to parse --test-bool";
    return EXIT_FAILURE;
    }

  // Test3 - check if adding QString, int, and QStringList arguments works
  QStringList arguments3;
  arguments3 << "ctkCommandLineParserTest1";
  arguments3 << "--test-string" << "TestingIsGood";
  arguments3 << "--test-string2"<< "CTKSuperRocks";
  arguments3 << "--test-integer"<< "-3";
  arguments3 << "--test-stringlist"<< "item1" << "item2" << "item3";
  ctkCommandLineParser parser3;
  parser3.addArgument("--test-string", "", QVariant::String, "This is a test string");
  parser3.addArgument("--test-string2", "", QVariant::String, "This is a test string2", "CTKGood");
  parser3.addArgument("--test-integer", "", QVariant::Int, "This is a test integer");
  parser3.addArgument("--test-stringlist", "", QVariant::StringList,
                                "This is a test stringlist");
  ok = false;
  parsedArgs = parser3.parseArguments(arguments3, &ok);
  if (!ok)
    {
    qCritical() << "Test3 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  QString expectedTestString = "TestingIsGood";
  if (parsedArgs["--test-string"].toString() != expectedTestString)
    {
    qCritical() << "Test3 - Failed - testString" << parsedArgs["--test-string"].toString()
        << ", expectedTestString" << expectedTestString;
    return EXIT_FAILURE;
    }

  QString expectedTestString2 = "CTKSuperRocks";
  if (parsedArgs["--test-string2"].toString() != expectedTestString2)
    {
    qCritical() << "Test3 - Failed - testString2" << parsedArgs["--test-string2"].toString()
        << ", expectedTestString2" << expectedTestString2;
    return EXIT_FAILURE;
    }

  int expectedTestInteger = -3;
  if (parsedArgs["--test-integer"].toInt() != expectedTestInteger)
    {
    qCritical() << "Test3 - Failed - testInteger" << parsedArgs["--test-integer"].toInt()
        << ", expectedTestInteger" << expectedTestInteger;
    return EXIT_FAILURE;
    }

  QStringList expectedTestStringlist;
  expectedTestStringlist << "item1" << "item2" << "item3";
  if (parsedArgs["--test-stringlist"].toStringList() != expectedTestStringlist)
    {
    qCritical() << "Test3 - Failed - testStringlist" << parsedArgs["--test-stringlist"].toStringList()
        << ", expectedTestStringlist" << expectedTestStringlist;
    return EXIT_FAILURE;
    }

  // Test4 - check if helpText() works as expected
  ctkCommandLineParser parser4;
  parser4.addArgument("--help-string", "", QVariant::String, "This is an help string");
  parser4.addArgument("--help-string-med", "", QVariant::String, "");
  parser4.addArgument("--help-string-long", "-hs2", QVariant::String, "This is an help string too !");
  parser4.addArgument("", "-hs3", QVariant::String, "This is an help string too for sure !?");

  QString expectedHelpString;
  QTextStream streamExpectedHelpString(&expectedHelpString);
  streamExpectedHelpString << "  --help-string..........This is an help string\n"
                            << "  --help-string-med\n"
                            << "  -hs2\n"
                            << "  --help-string-long.....This is an help string too !\n"
                            << "  -hs3...................This is an help string too for sure !?\n";

  if (expectedHelpString != parser4.helpText('.'))
    {
    qCritical() << "Test4 - Problem with helpText('.') - helpText:\n" << parser4.helpText('.')
        << ", expectedHelpString:\n" << expectedHelpString;
    return EXIT_FAILURE;
    }

  QString expectedHelpString2;
  QTextStream streamExpectedHelpString2(&expectedHelpString2);
  streamExpectedHelpString2 << "  --help-string          This is an help string\n"
                           << "  --help-string-med\n"
                           << "  -hs2\n"
                           << "  --help-string-long     This is an help string too !\n"
                           << "  -hs3                   This is an help string too for sure !?\n";
  if (expectedHelpString2 != parser4.helpText())
    {
    qCritical() << "Test4 - Problem with helpText() - helpText:\n" << parser4.helpText()
        << ", expectedHelpString2:\n" << expectedHelpString2;
    return EXIT_FAILURE;
    }

  // Test5 - check if setExactMatchRegularExpression() works as expected
  ctkCommandLineParser parser5;

  if (parser5.setExactMatchRegularExpression("--unknown",".*", "invalid"))
    {
    qCritical() << "Test5 - Problem with setExactMatchRegularExpression(shortOrLongArg) - "
                   "The function should return false if an invalid argument is passed";
    return EXIT_FAILURE;
    }

  parser5.addArgument("--list", "", QVariant::StringList, "Test5 list");
  if (!parser5.setExactMatchRegularExpression("--list","item[0-9]",
                                              "Element of the form item[0-9] are expected."))
    {
    qCritical() << "Test5 - Problem with setExactMatchRegularExpression(StringListArg)";
    return EXIT_FAILURE;
    }
  parser5.addArgument("--string", "", QVariant::String, "Test5 string");
  if (!parser5.setExactMatchRegularExpression("--string","ctkStop|ctkStart",
                                              "ctkStop or ctkStart is expected."))
    {
    qCritical() << "Test5 - Problem with setExactMatchRegularExpression(StringArg)";
    return EXIT_FAILURE;
    }
  parser5.addArgument("--bool", "", QVariant::Bool, "Test5 bool");
  if (parser5.setExactMatchRegularExpression("--bool",".*", "invalid"))
    {
    qCritical() << "Test5 - Problem with setExactMatchRegularExpression(BooleanArg) - "
                   "The function should return false if a boolean argument is passed";
    return EXIT_FAILURE;
    }
  parser5.addArgument("--int", "", QVariant::Int, "Test5 int");
  if (!parser5.setExactMatchRegularExpression("--int","[1-3]",
                                              "Value 1, 2 or 3 is expected."))
    {
    qCritical() << "Test5 - Problem with setExactMatchRegularExpression(IntegerArg)";
    return EXIT_FAILURE;
    }

  QStringList arguments5;
  arguments5 << "ctkCommandLineParserTest1";
  arguments5 << "--string"<< "ctkStop";
  arguments5 << "--int"<< "2";
  arguments5 << "--list"<< "item2" << "item3";
  
  ok = false;
  parser5.parseArguments(arguments5, &ok);
  if (!ok)
    {
    qCritical() << "Test5 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  arguments5.clear();
  arguments5 << "ctkCommandLineParserTest1";
  arguments5 << "--string"<< "ctkStopp";
  arguments5 << "--int"<< "2";
  arguments5 << "--list"<< "item2" << "item3";

  ok = false;
  parser5.parseArguments(arguments5, &ok);
  if (ok)
    {
    qCritical() << "Test5 - parseArguments() should return False - 'ctkStopp' isn't a valid string";
    return EXIT_FAILURE;
    }

  QString expectedErrorString =
    "Value(s) associated with argument --string are incorrect."
    " ctkStop or ctkStart is expected.";

  if(expectedErrorString != parser5.errorString())
    {
    qCritical() << "Test5 - Failed - expectedErrorString" << expectedErrorString
            << ", parser5.errorString()" << parser5.errorString();
    return EXIT_FAILURE;
    }

  arguments5.clear();
  arguments5 << "ctkCommandLineParserTest1";
  arguments5 << "--string"<< "ctkStop";
  arguments5 << "--int"<< "4";
  arguments5 << "--list"<< "item2" << "item3";

  ok = false;
  parser5.parseArguments(arguments5, &ok);
  if (ok)
    {
    qCritical() << "Test5 - parseArguments() should return False - '4' isn't a valid int";
    return EXIT_FAILURE;
    }

  QString expectedErrorString2 =
    "Value(s) associated with argument --int are incorrect."
    " Value 1, 2 or 3 is expected.";

  if(expectedErrorString2 != parser5.errorString())
    {
    qCritical() << "Test5 - Failed - expectedErrorString2" << expectedErrorString2
            << ", parser5.errorString()" << parser5.errorString();
    return EXIT_FAILURE;
    }

  arguments5.clear();
  arguments5 << "ctkCommandLineParserTest1";
  arguments5 << "--string"<< "ctkStop";
  arguments5 << "--int"<< "2";
  arguments5 << "--list"<< "item2" << "item10";

  ok = false;
  parser5.parseArguments(arguments5, &ok);
  if (ok)
    {
    qCritical() << "Test5 - parseArguments() should return False "
                   "- 'item10' isn't a valid list element";
    return EXIT_FAILURE;
    }

  QString expectedErrorString3 =
    "Value(s) associated with argument --list are incorrect."
    " Element of the form item[0-9] are expected.";

  if(expectedErrorString3 != parser5.errorString())
    {
    qCritical() << "Test5 - Failed - expectedErrorString3" << expectedErrorString3
            << ", parser5.errorString()" << parser5.errorString();
    return EXIT_FAILURE;
    }
    
  // Test6 - Check if the parser handle the case when value of parameter is omitted
  ctkCommandLineParser parser6;
  parser6.addArgument("--string", "", QVariant::String, "This is a string");
  parser6.addArgument("--bool", "", QVariant::Bool, "This is a bool");
  
  QStringList arguments6; 
  arguments6 << "ctkCommandLineParserTest1"
             << "--string";
  
  // since --string is missing a parameter, parseArgument is expected to fail
  ok = false;
  parser6.parseArguments(arguments6, &ok);
  if (ok)
    {
    qCritical() << "Test6 - Problem with parseArguments()";
    return EXIT_FAILURE;
    }

  // Expected ignore argument for Test7, 8. 9 and 10
  QStringList expectedUnparsedArguments;
  expectedUnparsedArguments << "--ignoreint"<< "2" << "--ignorelist"<< "item1" << "item2";

  // Test7 - Check if addBooleanArgument and ignore_rest=true works as expected
  ctkCommandLineParser parser7;
  bool test7Bool = false;
  parser7.addArgument("--bool", "", QVariant::Bool, "This is a boolean",
                             false /*defaultValue*/, true /* ignoreRest*/);
  QStringList arguments7;
  arguments7 << "ctkCommandLineParserTest1";
  arguments7 << "--bool";
  arguments7 << expectedUnparsedArguments;

  ok = false;
  parsedArgs = parser7.parseArguments(arguments7, &ok);
  if (!ok)
    {
    qCritical() << "Test7 - Failed to parse arguments";
    return EXIT_FAILURE;
    }
  bool expectedTest7Bool = true;
  if (parsedArgs["--bool"].toBool() != expectedTest7Bool)
    {
    qCritical() << "Test7 - Failed - test7Bool" << test7Bool
        << ", expectedTest7Bool" << expectedTest7Bool;
    return EXIT_FAILURE;
    }

  if (parser7.unparsedArguments() != expectedUnparsedArguments)
    {
    qCritical() << "Test7 - Failed - expectedUnparsedArguments " << expectedUnparsedArguments
                << ", parser7.unparsedArguments" << parser7.unparsedArguments();
    return EXIT_FAILURE;
    }

  // Test8 - Check if addStringArgument and ignore_rest=true works as expected
  ctkCommandLineParser parser8;
  parser8.addArgument("--string", "", QVariant::String, "This is a string",
                            QString() /*defaultValue*/, true /* ignoreRest*/);

  QStringList arguments8;
  arguments8 << "ctkCommandLineParserTest1";
  arguments8 << "--string" << "ctk";
  arguments8 << expectedUnparsedArguments;

  ok = false;
  parsedArgs = parser8.parseArguments(arguments8, &ok);
  if (!ok)
    {
    qCritical() << "Test8 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  QString expectedTest8String = "ctk";
  if (parsedArgs["--string"].toString() != expectedTest8String)
    {
    qCritical() << "Test8 - Failed - test8String" << parsedArgs["--string"].toString()
        << ", expectedTest8String" << expectedTest8String;
    return EXIT_FAILURE;
    }

  if (parser8.unparsedArguments() != expectedUnparsedArguments)
    {
    qCritical() << "Test8 - Failed - expectedUnparsedArguments " << expectedUnparsedArguments
                << ", parser8.unparsedArguments" << parser8.unparsedArguments();
    return EXIT_FAILURE;
    }

  // Test9 - Check if addArgument for int and ignore_rest=true works as expected
  ctkCommandLineParser parser9;
  parser9.addArgument("--integer", "", QVariant::Int, "This is an integer",
                             0 /*defaultValue*/, true /* ignoreRest*/);

  QStringList arguments9;
  arguments9 << "ctkCommandLineParserTest1";
  arguments9 << "--integer" << "74";
  arguments9 << expectedUnparsedArguments;

  ok = false;
  parsedArgs = parser9.parseArguments(arguments9, &ok);
  if (!ok)
    {
    qCritical() << "Test9 - Failed to parse arguments";
    return EXIT_FAILURE;
    }

  int expectedTest9Int = 74;
  if (parsedArgs["--integer"].toInt() != expectedTest9Int)
    {
    qCritical() << "Test9 - Failed - test9Int" << parsedArgs["--integer"].toInt()
        << ", expectedTest9Int" << expectedTest9Int;
    return EXIT_FAILURE;
    }

  if (parser9.unparsedArguments() != expectedUnparsedArguments)
    {
    qCritical() << "Test9 - Failed - expectedUnparsedArguments " << expectedUnparsedArguments
                << ", parser9.unparsedArguments" << parser9.unparsedArguments();
    return EXIT_FAILURE;
    }

  // Test10 - Check if argumentParsed works as expected
  ctkCommandLineParser parser10;
  parser10.addArgument("--bool", "", QVariant::Bool, "This is a boolean");

  // Since parseArguments hasn't been called, argumentParsed should return False
  if(parser10.argumentParsed("--bool"))
    {
    qCritical() << "Test10 - Problem with argumentParsed() - Should return False";
    return EXIT_FAILURE;
    }

  QStringList arguments10;
  arguments10 << "ctkCommandLineParserTest1";
  arguments10 << "--bool";

  ok = false;
  parser10.parseArguments(arguments10, &ok);
  if (!ok)
    {
    qCritical() << "Test10 - Failed to parse arguments.";
    return EXIT_FAILURE;
    }

  if(parser10.argumentParsed("--bool-notadded"))
    {
    qCritical() << "Test10 - Problem with argumentParsed() - "
                   "Should return False since argument '--bool-notadded' hasn't been added.";
    return EXIT_FAILURE;
    }

  if(!parser10.argumentParsed("--bool"))
    {
    qCritical() << "Test10 - Problem with argumentParsed() - Should return True";
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
