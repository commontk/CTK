
// CTK includes
#include "ctkFileLogger.h"
#include "ctkTest.h"

// ----------------------------------------------------------------------------
class ctkFileLoggerTester: public QObject
{
  Q_OBJECT
public:
  explicit ctkFileLoggerTester(QObject* parent = nullptr) : QObject(parent) {}
private slots:
  void initTestCase();

};

// ----------------------------------------------------------------------------
void ctkFileLoggerTester::initTestCase()
{

}

// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkFileLoggerTest)
#include "ctkFileLoggerTest.moc"
