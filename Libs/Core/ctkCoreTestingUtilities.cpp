
#include "ctkCoreTestingUtilities.h"

namespace ctkCoreTestingUtilities
{

//----------------------------------------------------------------------------
bool CheckInt(int line, const QString& description,
              int current, int expected)
{
  return Check<int>(line, description, current, expected, "CheckInt");
}

//----------------------------------------------------------------------------
bool CheckNotNull(int line, const QString& description,
                  const void* pointer)
{
  if(!pointer)
    {
    qWarning() << "\nLine " << line << " - " << description
               << " : CheckNotNull failed"
               << "\n\tpointer:" << pointer;
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool CheckNull(int line, const QString& description, const void* pointer)
{
  if(pointer)
    {
    qWarning() << "\nLine " << line << " - " << description
               << " : CheckNull failed"
               << "\n\tpointer:" << pointer;
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool CheckPointer(int line, const QString& description,
                  void* current, void* expected, bool errorIfDifferent /* = true */)
{
  return Check<void*>(line, description, current, expected, "CheckPointer", errorIfDifferent);
}

//----------------------------------------------------------------------------
bool CheckString(int line, const QString& description,
                 const char* current, const char* expected, bool errorIfDifferent /* = true */)
{
  QString testName = "CheckString";

  bool different = true;
  if (current == 0 || expected == 0)
    {
    different = !(current == 0 && expected == 0);
    }
  else if(strcmp(current, expected) == 0)
    {
    different = false;
    }
  if(different == errorIfDifferent)
    {
    qWarning() << "\nLine " << line << " - " << description
               << " : " << testName << "  failed"
               << "\n\tcurrent :" << (current ? current : "<null>")
               << "\n\texpected:" << (expected ? expected : "<null>");
    return false;
    }
  return true;
}

//----------------------------------------------------------------------------
bool CheckStringList(int line, const QString& description,
                     const QStringList& current, const QStringList& expected)
{
  return CheckList<QString>(line, description, current, expected, "CheckStringList");
}

//----------------------------------------------------------------------------
bool CheckVariant(int line, const QString& description,
                  const QVariant& current, const QVariant& expected)
{
  return Check<QVariant>(line, description, current, expected, "CheckVariant");
}

} // namespace ctkCoreTestingUtilities

