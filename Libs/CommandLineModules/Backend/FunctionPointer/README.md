Function Pointer (experimental)    {#CommandLineModulesBackendFunctionPointer_Page}
===============================

\internal This page is best viewed in its [Doxygen processed]
(http://commontk.org/docs/html/CommandLineModulesBackendFunctionPointer_Page.html) form. \endinternal

The Function Pointer back-end is an experimental *proof-of-concept* (however, it is used in
some unit tests as a light-weight back-end).

This back-end allows the registration of C/C++ function pointers and uses template magic
to auto-generate an XML description using the type information in the function signature.
Many template specializations for certain parameter types are missing, hence this back-end
will only work for a limited set of argument types. See the ctkCmdLineModuleBackendFunctionPointer
class for more information.

See the \ref CommandLineModulesBackendFunctionPointer_API module for the API documentation.
