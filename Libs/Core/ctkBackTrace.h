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

#ifndef __ctkBackTrace_h
#define __ctkBackTrace_h

// CTK includes
#include <ctkCoreExport.h>

// Qt includes
#include <QScopedPointer>
#include <QString>


struct ctkBackTracePrivate;

//---------------------------------------------------------------------------
/**
 * \ingroup Core
 *
 * \brief Obtains a back trace from the current execution context.
 *
 * \remarks It is generally not safe to use this class in signal handlers.
 */
class CTK_CORE_EXPORT ctkBackTrace
{
public:

  static size_t const DefaultStackSize;

  ctkBackTrace(const ctkBackTrace& other);

  /**
   * \brief Create a back trace.
   * \param framesNumber The default maximum stack size.
   */
  ctkBackTrace(size_t framesNumber = DefaultStackSize);

  virtual ~ctkBackTrace() throw();

  /**
   * @brief Get the stack size.
   * @return The number of stack frames for this back trace.
   */
  size_t stackSize() const;

  /**
   * @brief Get the return address for a given stack frame.
   * @param frameNumber The stack frame number.
   * @return The return address for the stack frame with number <code>frameNumber</code>
   *         or <code>NULL</code> if there is no corresponding stack frame.
   */
  void* returnAddress(unsigned frameNumber) const;

  /**
   * @brief Get a textual representation for a given stack frame.
   * @param frameNumber The stack frame number.
   * @return A string describing the stack frame with number <code>frameNumber</code>
   *         or a null QString if there is no corresponding stack frame.
   */
  QString stackFrame(unsigned frameNumber) const;

  /**
   * @brief Provides programmatic access to the stack trace information.
   *
   * The zeroth element of the returned list (assuming the list's size is non-zero)
   * represents the top of the stack, which is the last method invocation in the sequence.
   *
   * @return A list of string representations for each stack frame.
   */
  QList<QString> stackTrace() const;

private:

  QScopedPointer<ctkBackTracePrivate> d;
};

#ifdef Q_CC_MSVC
namespace ctk {
CTK_CORE_EXPORT bool DebugSymInitialize();
}
#endif

#endif // __ctkBackTrace_h
