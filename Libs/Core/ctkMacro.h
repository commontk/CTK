#ifndef __ctkMacro_h
#define __ctkMacro_h

// Define CTK_PRAGMA macro.
//
// It sets "#pragma" preprocessor directives without expecting the arguments
// to be quoted.
#define CTK_PRAGMA(x) _Pragma (#x)

// Define CTK_GCC_PRAGMA_DIAG(param1 [param2 [...]]) macro.
//
// This macros sets a pragma diagnostic if it supported by the version
// of GCC being used otherwise it is a no-op.
//
// GCC diagnostics pragma supported only with GCC >= 4.2
#if defined( __GNUC__ ) && !defined( __INTEL_COMPILER )
#  if ( __GNUC__ > 4 ) || (( __GNUC__ >= 4 ) && ( __GNUC_MINOR__ >= 2 ))
#    define CTK_GCC_PRAGMA_DIAG(x) CTK_PRAGMA(GCC diagnostic x)
#  else
#    define CTK_GCC_PRAGMA_DIAG(x)
#  endif
#else
#  define CTK_GCC_PRAGMA_DIAG(x)
#endif

// Define CTK_GCC_PRAGMA_DIAG_(PUSH|POP) macros.
//
// These macros respectively push and pop the diagnostic context
// if it is supported by the version of GCC being used
// otherwise it is a no-op.
//
// GCC push/pop diagnostics pragma are supported only with GCC >= 4.6
//
// Define macro CTK_HAS_GCC_PRAGMA_DIAG_PUSHPOP if it is supported.
#if defined( __GNUC__ ) && !defined( __INTEL_COMPILER )
#  if ( __GNUC__ > 4 ) || (( __GNUC__ >= 4 ) && ( __GNUC_MINOR__ >= 6 ))
#    define CTK_GCC_PRAGMA_DIAG_PUSH() CTK_GCC_PRAGMA_DIAG(push)
#    define CTK_GCC_PRAGMA_DIAG_POP() CTK_GCC_PRAGMA_DIAG(pop)
#    define CTK_HAS_GCC_PRAGMA_DIAG_PUSHPOP
#  else
#    define CTK_GCC_PRAGMA_DIAG_PUSH()
#    define CTK_GCC_PRAGMA_DIAG_POP()
#  endif
#else
#  define CTK_GCC_PRAGMA_DIAG_PUSH()
#  define CTK_GCC_PRAGMA_DIAG_POP()
#endif

#endif
