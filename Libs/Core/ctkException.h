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


#ifndef __ctkException_h
#define __ctkException_h

// Qt includes
#include <QString>

// CTK includes
#include <ctkCoreExport.h>

//---------------------------------------------------------------------------
/**
 * \ingroup Core
 *
 * \brief The base class for all exceptions defined in CTK.
 *
 * This exception class takes a QString object as the message text and can
 * optionally store another ctkException instance which caused this exception.
 *
 * ctkException classes can be copied, saved, and rethrown.
 */
class CTK_CORE_EXPORT ctkException : public std::exception
{
public:

  /**
   * @brief Create a new ctkException.
   * @param msg The exception message.
   */
  ctkException(const QString& msg);

  /**
   * @brief Create a new ctkException containing another exception as the cause.
   * @param msg The exception message.
   * @param cause The nested exception causing this exception.
   */
  ctkException(const QString& msg, const ctkException& cause);

  /**
   * @brief Copy constructor.
   * @param o The exception to copy.
   */
  ctkException(const ctkException& o);

  /**
   * @brief Assignment operator.
   * @param o The exception to assign to this exception.
   * @return
   */
  ctkException& operator=(const ctkException& o);

  ~ctkException() throw();

  /**
   * @brief Gets the nested exception which caused this exception.
   * @return The nested exception, or <code>NULL</code> if there is none.
   */
  const ctkException* cause() const throw();

  /**
   * @brief Sets the cause for this exception.
   * @param cause The exception causing this exception.
   */
  void setCause(const ctkException& cause);

  /**
   * @brief Returns the name for this exception.
   * @return The exception name.
   */
  virtual const char* name() const throw();

  /**
   * @brief Returns the class name for this exception.
   * @return The exception class name.
   */
  virtual const char* className() const throw();

  /**
   * @brief Returns a static string describing this exception.
   * @return The exception description.
   */
  virtual const char* what() const throw();

  /**
   * @brief Creates a copy of this exception. Use rethrow() to throw the
   * copy again.
   * @return A copy of this exception.
   */
  virtual ctkException* clone() const;

  /**
   * @brief (Re)Throws this exception.
   */
  void rethrow() const;

private:

  QString Msg;
  ctkException* NestedException;
};

//---------------------------------------------------------------------------
/**
 * \ingroup Core
 */
CTK_CORE_EXPORT QDebug operator<<(QDebug dbg, const ctkException& exc);

//---------------------------------------------------------------------------
/**
 * \ingroup Core
 *
 * \brief Quickly declare a ctkException sub-class.
 * \param API The export macro.
 * \param CLS The class name for the ctkException sub-class.
 * \param BASE The class name of the actual super class.
 */
#define CTK_DECLARE_EXCEPTION(API, CLS, BASE)         \
  class API CLS : public BASE                         \
  {                                                   \
  public:                                             \
    CLS(const QString& msg);                          \
    CLS(const QString& msg, const ctkException& exc); \
    CLS(const CLS& exc);                              \
    ~CLS() throw();                                   \
    CLS& operator = (const CLS& exc);                 \
    const char* name() const throw();                 \
    CLS* clone() const;                               \
  };

//---------------------------------------------------------------------------
/**
 * \ingroup Core
 *
 * \brief Quickly implement a ctkException sub-class
 * \param CLS The class name for the ctkException sub-class.
 * \param BASE The class name of the actual super class.
 * \param NAME A human-readable name for this exception class.
 */
#define CTK_IMPLEMENT_EXCEPTION(CLS, BASE, NAME)                         \
  CLS::CLS(const QString& msg) : BASE(msg)                               \
  { }                                                                    \
  CLS::CLS(const QString& msg, const ctkException& exc) : BASE(msg, exc) \
  { }                                                                    \
  CLS::CLS(const CLS& exc) : BASE(exc)                                   \
  { }                                                                    \
  CLS::~CLS() throw()                                                    \
  { }                                                                    \
  CLS& CLS::operator = (const CLS& exc)                                  \
  {                                                                      \
    BASE::operator = (exc);                                              \
    return *this;                                                        \
  }                                                                      \
  const char* CLS::name() const throw()                                  \
  {                                                                      \
    return NAME;                                                         \
  }                                                                      \
  ctkException* CLS::clone() const                                       \
  {                                                                      \
    return new CLS(*this);                                               \
  }

#endif // __ctkException_h
