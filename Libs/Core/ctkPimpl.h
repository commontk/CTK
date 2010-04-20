/**
\class ctkPimpl ctkPimpl
\brief Hide private details of a class

Application code generally doesn't have to be concerned about hiding its
implementation details, but when writing library code it is important to
maintain a constant interface, both source and binary. Maintaining a constant
source interface is easy enough, but keeping the binary interface constant
means moving implementation details into a private class. The PIMPL, or
d-pointer, idiom is a common method of implementing this separation. ctkPimpl
offers a convenient way to connect the public and private sides of your class.

\section start Getting Started
Before you declare the public class, you need to make a forward declaration
of the private class. The private class must have the same name as the public
class, followed by the word Private.

\subsection pub The Public Class
Generally, you shouldn't keep any data members in the public class without a
good reason. Functions that are part of the public interface should be declared
in the public class, and functions that need to be available to subclasses (for
calling or overriding) should be in the protected section of the public class.
To connect the private class to the public class, include the
QCTK_DECLARE_PRIVATE macro in the private section of the public class. In the
example above, the private class is connected as follows:

Additionally, you must include the QCTK_INIT_PRIVATE macro in the public class's
constructor.

\subsection priv The Private Class
As mentioned above, data members should usually be kept in the private class.
This allows the memory layout of the private class to change without breaking
binary compatibility for the public class. Functions that exist only as
implementation details, or functions that need access to private data members,
should be implemented here.

To define the private class, inherit from the template qCTKPrivate class, and
include the QCTK_DECLARE_PUBLIC macro in its public section. The template
parameter should be the name of the public class.

\section cross Accessing Private Members
Use the qctk_d() function (actually a function-like object) from functions in
the public class to access the private class. Similarly, functions in the
private class can invoke functions in the public class by using the qctk_p()
function (this one's actually a function).
*/

#ifndef __ctkPimpl_h
#define __ctkPimpl_h

// Qt includes
#include <QtGlobal>

/*! \relates ctkPimpl
 * Define a public class constructor with no argument
 *
 * Also make sure the Pimpl is initalized
 */
#define CTK_CONSTRUCTOR_NO_ARG_CXX(PUB)  \
  PUB::PUB()                             \
    {                                    \
    CTK_INIT_PRIVATE(PUB);               \
    }

/*! \relates ctkPimpl
 * Define a public class constructor with one argument
 *
 * Also make sure the Pimpl is initalized
 */
#define CTK_CONSTRUCTOR_1_ARG_CXX(PUB, _ARG1)   \
  PUB::PUB(_ARG1 _parent) :                     \
    Superclass( _parent )                       \
    {                                           \
    CTK_INIT_PRIVATE(PUB);                      \
    }
    
/*! \relates ctkPimpl
 * Define the setter in the public class.
 *
 * This should be put in the .cxx file of the public class. The parameter are
 * the name of the public class (PUB), the type of the argument to return (_TYPE),
 * the name of the getter(_NAME) and the name of the variable in the Private class(_VARNAME).
 */
#define CTK_SET_CXX(PUB, _TYPE, _NAME, _VARNAME)    \
  void PUB::_NAME(_TYPE var)                        \
  {                                                 \
    ctk_d.ref()._VARNAME =  var;                    \
  }

/*! \relates ctkPimpl
 * Define the setter in the public class.
 *
 * This should be put in the .cxx file of the public class. The parameter are
 * the name of the public class (PUB), the type of the argument to return (_TYPE),
 * the name of the setter(_NAME) and the name of the variable in the Private class(_VARNAME).
 */
#define CTK_GET_CXX(PUB, _TYPE, _NAME, _VARNAME)   \
  _TYPE PUB::_NAME()const                          \
  {                                                \
    return ctk_d.ref()._VARNAME;                   \
  }

/*! \relates ctkPimpl
 * Declares that a public class has a related private class.
 *
 * This should be put in the private section of the public class. The parameter is the name of the public class.
 * For convenience, this macro also add 'typedef PUB Self;'
 */
#define CTK_DECLARE_PRIVATE(PUB)                                          \
friend class PUB##Private; ctkPrivateInterface<PUB, PUB##Private> ctk_d;  \
typedef PUB Self;

/*! \relates ctkPimpl
 * Declares that a private class has a related public class.
 *
 * This may be put anywhere in the declaration of the private class. The parameter is the name of the public class.
 */
#define CTK_DECLARE_PUBLIC(PUB) friend class PUB;
/*! \relates ctkPimpl
 * Initializes resources owned by the private class.
 *
 * This should be called from the public class's constructor,
 * before qctk_d() is used for the first time. The parameter is the name of the public class.
 */
#define CTK_INIT_PRIVATE(PUB) ctk_d.setPublic(this)
/*! \relates ctkPimpl
 * Returns a pointer (or reference) in the current scope named "d" to the private class.
 *
 * This function is only available in a class using \a QCTK_DECLARE_PRIVATE.
 */
#define CTK_D(PUB) PUB##Private* d = ctk_d()
#define CTK_D_REF(PUB) PUB##Private& d = ctk_d.ref()
/*! \relates ctkPimpl
 * Creates a pointer ( or reference) in the current scope named "q" to the public class.
 *
 * This macro only works in a class using \a CTK_DECLARE_PUBLIC.
 */
#define CTK_P(PUB) PUB* p = ctk_p()
#define CTK_P_REF(PUB) PUB& p = ctk_p_ref()

#ifdef CTK_DOXYGEN_RUN
/*! \relates ctkPimpl
 * Returns a pointer to the private class.
 *
 * This function is only available in a class using \a CTK_DECLARE_PRIVATE.
 */
ctkPrivate<PUB>* ctk_d();

/*! \relates ctkPimpl
 * Returns a const pointer to the private class.
 *
 * This function is only available in a class using \a CTK_DECLARE_PRIVATE.
 * This overload will be automatically used in const functions.
 */
const ctkPrivate<PUB>* ctk_d();

/*! \relates ctkPimpl
 * Returns a reference or pointer to the public class.
 *
 * This function is only available in a class using \a CTK_DECLARE_PUBLIC.
 */
PUB& ctk_p_ref();
PUB* ctk_p();

/*! \relates ctkPimpl
 * Returns a const reference or pointer to the public class.
 *
 * This function is only available in a class using \a CTK_DECLARE_PUBLIC.
 * This overload will be automatically used in const functions.
 */
const PUB& ctk_p_ref();
const PUB* ctk_p();
#endif

template <typename PUB>
class ctkPrivate
{
public:
  virtual ~ctkPrivate(){}
  inline void CTK_setPublic(PUB* pub)
    {
    Q_ASSERT(pub);
    ctk_p_ptr = pub;
    }

protected:
  inline PUB& ctk_p_ref()
    {
    Q_ASSERT(this->ctk_p_ptr);
    return *this->ctk_p_ptr;
    }
  inline const PUB& ctk_p_ref() const
    {
    Q_ASSERT(this->ctk_p_ptr);
    return *this->ctk_p_ptr;
    }

  inline PUB* ctk_p()
    {
    Q_ASSERT(this->ctk_p_ptr);
    return this->ctk_p_ptr;
    }
  inline const PUB* ctk_p() const
    {
    Q_ASSERT(this->ctk_p_ptr);
    return this->ctk_p_ptr;
    }

private:
  PUB* ctk_p_ptr;
};

template <typename PUB, typename PVT>
class ctkPrivateInterface
{
  friend class ctkPrivate<PUB>;
public:
  ctkPrivateInterface()
    {
    this->pvt = new PVT;
    }
  ~ctkPrivateInterface()
    {
    delete this->pvt;
    }

  inline void setPublic(PUB* pub)
    {
    Q_ASSERT(pub);
    this->pvt->CTK_setPublic(pub);
    }
  inline PVT& ref()
    {
    return *static_cast<PVT*>(this->pvt);
    }
  inline const PVT& ref() const
    {
    return *static_cast<PVT*>(this->pvt);
    }
  inline PVT* operator()()
    {
    return static_cast<PVT*>(this->pvt);
    }
  inline const PVT* operator()() const
    {
    return static_cast<PVT*>(this->pvt);
    }
private:
  ctkPrivate<PUB>* pvt;
};

#endif
