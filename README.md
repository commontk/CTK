# Common Toolkit

[![CircleCI Status][circleci-badge]][circleci-link]
[![License][license-badge]][license-link]

[circleci-badge]: https://dl.circleci.com/status-badge/img/gh/commontk/CTK/tree/master.svg?style=shield
[circleci-link]: https://dl.circleci.com/status-badge/redirect/gh/commontk/CTK/tree/master

[license-badge]: https://img.shields.io/github/license/commontk/CTK?color=blue
[license-link]: https://github.com/commontk/CTK/blob/master/LICENSE

The Common Toolkit (CTK) is a community-driven effort, focused on providing support code for
medical image analysis, surgical navigation, and related projects.

## Overview

CTK covers topics not addressed by existing toolkits, catering to the mutual interests and needs of the CTK community. The current scope of CTK efforts includes the following topics: _DICOM_, _DICOM Application Hosting_, _Widgets_, _Plugin Framework_ and _Command Line Interfaces_.

## Getting Started

To work with CTK, you need to have a C++ compiler, Qt libraries, and CMake installed.

### Configure

Configure the project using CMake by setting the following options:
  * `CTK_QT_VERSION`: 5
  * `Qt5_DIR`: `C:\Qt\5.15.2\msvc2019_64\lib\cmake\Qt5` (or a similar path, depending on your operating system)

### Notes

* To install the required development environment easily, refer to the "Prerequisites" section of the
  3D Slicer [build instructions](https://slicer.readthedocs.io/en/latest/developer_guide/build_instructions/index.html).

* If you set `CTK_LIB_Visualization/VTK/Widgets` to `ON`, make sure to configure VTK by enabling the following modules:
  * `VTK_MODULE_ENABLE_VTK_ChartsCore`: YES (to enable VTK charts)
  * `VTK_MODULE_ENABLE_VTK_GUISupportQt`: YES (to enable VTK widgets)
  * `VTK_MODULE_ENABLE_VTK_ViewsQt`: YES (to enable VTK view widgets)
  * For more information, see the [VTK Build Instructions](https://docs.vtk.org/en/latest/build_instructions/index.html) and [VTK Modules](https://docs.vtk.org/en/latest/modules/index.html) documentation.

* Make sure your built toolchain version is compatible with the chosen Qt version.
  For example if trying to build with Qt-5.12 and Microsoft Visual Studio 2019, then build will fail with the error `error LNK2019: unresolved external symbol "__declspec(dllimport) public: __cdecl QLinkedListData::QLinkedListData(void)"` while trying to build with Qt 5.12 and Microsoft Visual Studio 2019, the solution is to either change the toolset version to an earlier one (e.g., Visual Studio 2017) or upgrade Qt (e.g., use Qt 5.15 with Visual Studio 2019).

## Topics

### DICOM

CTK provides high-level classes that support query and retrieve operations from PACS and local databases. It includes Qt widgets for easily setting up a server connection, sending queries, and viewing the results. The underlying toolkit used is [DCMTK](https://dicom.offis.de/en/dcmtk/).

To learn more, refer to the [Overview of DICOM functionality in CTK](https://commontk.org/index.php/Documentation/Dicom_Overview).

### DICOM Application Hosting

CTK aims to create a C++ reference implementation of the [DICOM Part 19 Application Hosting specifications](https://commontk.org/images/8/8e/DicomAppHostingSpecs.pdf). It provides an infrastructure to create both hosts and hosted applications. Although the project is still in alpha status, it can be useful for conformance testing and initial experimentation.

To learn more, see the [DICOM Application Hosting](https://commontk.org/index.php/Documentation/DicomApplicationHosting) documentation.

### Widgets

CTK offers a collection of Qt Widgets for use in biomedical imaging applications.

To explore the available widgets, visit the [Gallery](https://commontk.org/index.php/Documentation/ImageGallery) and the associated [Widgets](https://commontk.org/index.php/Documentation/Widgets) wiki page.

### Plugin Framework

CTK provides a dynamic component system for C++, modeled after the [OSGi](http://www.osgi.org) specifications. It enables a development model where applications are dynamically composed of many different reusable components, following a service-oriented approach.

To get started with the Plugin Framework, refer to the [Plugin Framework](https://commontk.org/index.php/Documentation/Plugin_Framework) documentation.

### Command Line Interfaces

CTK supports the usage of algorithms written as self-contained executables in multiple programming languages. It includes a command line interface (CLI) module that simplifies the integration of command line tools into CTK-based applications. The CLI module provides a flexible and extensible framework for defining, executing, and configuring command line interfaces for algorithms.

To learn more about the CLI module, see the [Command Line Interfaces](https://commontk.org/index.php/Documentation/Command_Line_Interfaces) documentation.

## Supported Qt versions

CTK supports the following versions of Qt:

* Qt 5
  * CMake options:
    * `CTK_QT_VERSION` set to `5`
    * `Qt5_DIR` set to `C:\Qt\5.15.2\msvc2019_64\lib\cmake\Qt5` (or a similar path, depending on your operating system)
  * Requirements
    * C++11
    * VTK 9 or newer
    * PythonQt [patched-9](https://github.com/commontk/PythonQt/tree/patched-9) branch

Unsupported:

* Qt 4: The last CTK version to support Qt 4, C++98, VTK 8 and ITK v4.13.3 is [CTK 2023.07.13](https://github.com/commontk/CTK/releases/tag/2023.07.13).


## Supported Python versions

CTK supports the following versions of Python:

* Python 3

Unsupported:

* Python 2.7: The last CTK version to support Python 2.7 is [CTK 2023.07.13](https://github.com/commontk/CTK/releases/tag/2023.07.13).

## Dependency Management

CTK simplifies the process of managing dependencies by automatically handling the download, configuration, and building of required dependencies (except Qt) as part of the build process. This ensures that the necessary dependencies are readily available for building CTK-based applications without manual intervention.

To further support customization of CTK dependencies, you have the option to configure CTK by specifying options for each dependency. These options can include `<Package>_DIR` or `CTK_USE_SYSTEM_<Package>`, where `<Package>` represents the name of the respective dependency. For more details on the available options for each dependency, you can refer to the corresponding `CTK/CMakeExternals/<Package>.cmake` file.

In achieving this automatic dependency management, CTK internally integrates the [Artichoke](https://cmake-artichoke.readthedocs.io) CMake module, which enhances the built-in [ExternalProject](https://cmake.org/cmake/help/latest/module/ExternalProject.html) CMake module.

By utilizing this integrated approach, CTK facilitates the management and customization of dependencies, providing flexibility and ease of use for developers working with CTK-based applications.

## License

CTK code is licensed under Apache 2.0. This means that users of CTK are allowed to use the code for academic, commercial, or other purposes without paying license fees or being restricted in their ability to redistribute their code or keep it private. 

