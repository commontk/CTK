CTK Command Line Modules   {#CommandLineModules_Page}
========================

\brief Overview about the Command Line Modules support in CTK.

\internal This page is best viewed in its [Doxygen processed]
(http://commontk.org/docs/html/CommandLineModules_Page.html) form. \endinternal

[TOC]

CTK provides an API for interfacing with self-describing *runnable* modules which can provide an
XML description of their supported parameters. A runnable module is
usually (but not constrained to) a local executable and also referred to as a *command line module*.

The XML schema for the parameter description and most of the supported feature set for a module
has been adopted from the [Slicer Execution Model](http://www.slicer.org/slicerWiki/index.php/Slicer3:Execution_Model_Documentation).

The API provided by CTK allows the management, GUI generation, and asynchronous execution
of such modules in a toolkit-independent and interoperable way. Application writers can rely on the
provided libraries and their API to quickly integrate command line modules into their applications.

Developers who want to create command line modules which can be run by using the provided tools will
want to have a look at the *Creating Modules* section below. Everything else targets application writers
who want to host and mange these command line modules.

CTK also comes with an example application, called *ctkCommandLineModuleExplorer* which can be used
to load different kinds of modules, to verify their correctness, to run - and finally inspect their output.


Features
--------

Here is short overview about the provided feature set:

- Clear separation in front and back ends (see *Library Design* below)
- XML validation and error reporting
- Caching of XML descriptions
- Partially thread-safe, allowing to concurrently add and remove modules
- Asynchronous communication with running modules
  + Start/pause/cancel support (back-end and operating system dependent)
  + Result reporting
  + Output reporting

CTK also provides stable and feature-rich implementations of a Qt based front-end and a back-end handling local processes.

### Qt Gui front-end

The provided front-end implementation creates a Qt widgets based user interface. It also allows to customize the
GUI generation process, see the ctkCmdLineModuleFrontendQtGui class for more information.

### Local process back-end

The default back-end for running modules can handle local executables and runs them in a separate process. See
the ctkCmdLineModuleBackendLocalProcess class for details.


Creating Modules
----------------

Module writers usually need to create an XML file describing the parameters which are understood by the module (the actual
way how such an XML description is provided actually depends on the back-end for which the module is written). For locally
executable modules (e.g. command line programs), the XML description is usually emitted to the standard output channel by
the executable itself when it is called with a *--xml* command line argument.

The valid XML structure for the parameter description is defined in the corresponding [schema documentation](ctkCmdLineModule.xsd)
([absolute link](http://commontk.org/docs/html/ctkCmdLineModule.xsd)).

Please note that running a module may fail due to an invalid XML description. The strictness of validation is specific to the
application you are using to run the module. However, making sure the XML validates agains the given schema (raw schema file
[here](https://raw.github.com/commontk/CTK/master/Libs/CommandLineModules/Core/Resources/ctkCmdLineModule.xsd)).

### Progress and Result reporting

A module may report progress and intermediate results during its execution. The actual reporting mechanism depends on the type
of module. For a local executable being run for example by the ctkCmdLineModuleBackendLocalProcess back-end, reporting is done
by printing XML fragments to the standard output channel.

For example a progress report containing a progress value and text would look like:

    <filter-start>
      <filter-name>My Filter</filter-name>
      <filter-comment>Starting custom filter...</filter-comment>
    </filter-start>
    <filter-progress-text progress="0.2">Current progress: 0.2 (from [0,1.0])</filter-progress-text>
    <filter-end/>

Here is the XML [progress and result schema documentation](ctkCmdLineModuleProcess.xsd)
([absolute link](http://commontk.org/docs/html/ctkCmdLineModuleProcess.xsd)) describing the valid XML fragments. The raw
schema file is available [here](https://raw.github.com/commontk/CTK/master/Libs/CommandLineModules/Backend/LocalProcess/Resources/ctkCmdLineModuleProcess.xsd).


Library Design
--------------

The Command Line Module support consists of a \subpage CommandLineModulesCore_Page library and so-called
\subpage CommandLineModulesBackEnds_Page and \subpage CommandLineModulesFrontEnds_Page.

A front-end, a sub-class of ctkCmdLineModuleFrontend, represents a set of parameter values for a specific
module, usually associated with some kind of user interface. Front-end implementations need not be accessible outside
of the defining library, but may be exposed to allow the configuration of the GUI generation process by
sub-classing the corresponding ctkCmdLineModuleFrontendFactory implementation. A front-end can be "run" by
calling the ctkCmdLineModuleManager::run(ctkCmdLineModuleFrontend*) method and the object returned by the run()
method is used to communicate with the running module. A front-end can be "run" multiple times (with possibly different
parameter values) simultaneously.

A back-end, a sub-class of ctkCmdLineModuleBackend, knows how to actually "run" a module. Back-end implementations
express their capabilities by overriding the ctkCmdLineModuleBackend::schemes() method and providing a list of URL
schemes this back-end can handle. For example, the ctkCmdLineModuleBackendLocalProcess back-end returns "file" since
it can handle URLs pointing to local resources (executables). Further, a back-end knows how to get a time-stamp and
the module XML description for a specific module.

The central class for managing modules is the ctkCmdLineModuleManager. There must be at least one back-end registered
with the manager for module registrations to succeed. A module is registered by calling the
ctkCmdLineModuleManager::registerModule(const QUrl&) method, providing the URL to the module. If the URL scheme is not handled
by a previously registerd back-end, an exception is thrown. If registration succeeds, the method returns a
ctkCmdLineModuleReference object.

Creating specific front-ends for a given module is actually independent of the ctkCmdLineModuleManager, except that a
ctkCmdLineModuleReference object is needed. To create a front-end, usually the
ctkCmdLineModuleFrontendFactory::create(const ctkCmdLineModuleReference&) method is called, returning a
ctkCmdLineModuleFrontend pointer.

This separation of concerns in front and back ends allows for an extensible and flexible design. Front-ends and back-ends
work independent of each other and can be combined arbitrarly.


Quick Start
-----------

Here is a small code example to get you started quickly. We first instantiate a ctkCmdLineModuleManager object, using
a strict validation mode and the built-in caching mechanism.

\snippet ModuleManager/main.cpp instantiate-mm

Next, we instantiate and register a back-end.

\snippet ModuleManager/main.cpp register-backend

Now we register an executable as a module with the manager.

\snippet ModuleManager/main.cpp register-module

To create a front-end, we use the Qt widgets implementation.

\snippet ModuleManager/main.cpp create-frontend

Last, we run the front-end instance, using the default values for the module parameters.

\snippet ModuleManager/main.cpp run-module

After the ctkCmdLineModuleManager::run() method returns, we wait for the running module to finish and print out
some data reported by it.

