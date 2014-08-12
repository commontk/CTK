
// CTK includes
#include "ctkFileLogger.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkFileLoggerTester: public QObject
{
  Q_OBJECT
private slots:
  void initTestCase();

};

// ----------------------------------------------------------------------------
void ctkFileLoggerTester::initTestCase()
{

}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkFileLoggerTest)
#include "moc_ctkFileLoggerTest.cpp"
