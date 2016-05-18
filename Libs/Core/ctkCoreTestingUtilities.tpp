/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>

namespace ctkCoreTestingUtilities
{

//----------------------------------------------------------------------------
template<typename TYPE>
bool Check(int line, const QString& description,
           TYPE current, TYPE expected,
           const QString& _testName,
           bool errorIfDifferent = true)
{
  QString testName = _testName.isEmpty() ? "Check" : _testName;
  if (errorIfDifferent)
    {
    if(current != expected)
      {
      qWarning() << "\nLine " << line << " - " << description
                 << " : " << testName << " failed"
                 << "\n\tcurrent :" << current
                 << "\n\texpected:" << expected;
      return false;
      }
    }
  else
    {
    if(current == expected)
      {
      qWarning() << "\nLine " << line << " - " << description
                 << " : " << testName << " failed"
                 << "\n\tcurrent :" << current
                 << "\n\texpected to be different from:" << expected;
      return false;
      }
    }
  return true;
}

//----------------------------------------------------------------------------
template<typename TYPE>
bool CheckList(int line, const QString& description,
               const QList<TYPE>& current, const QList<TYPE>& expected,
               const QString& testName)
{
  QString msg;
  if (current.count() != expected.count())
    {
    qWarning() << "\nLine " << line << " - " << description
               << " : " << testName << " failed"
               << "\nCompared lists have different sizes."
               << "\n\tcurrent size :" << current.count()
               << "\n\texpected size:" << expected.count()
               << "\n\tcurrent:" << current
               << "\n\texpected:" << expected;
    return false;
    }
  for (int idx = 0; idx < current.count(); ++idx)
    {
    if (current.at(idx) != expected.at(idx))
      {
      qWarning() << "\nLine " << line << " - " << description
                 << " : " << testName << " failed"
                 << "\nCompared lists differ at index " << idx
                 << "\n\tcurrent[" << idx << "] :" << current.at(idx)
                 << "\n\texpected[" << idx << "]:" << expected.at(idx);
      return false;
      }
    }
  return true;
}

} // namespace ctkCoreTestingUtilities

