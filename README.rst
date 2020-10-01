Common Toolkit
==============

.. image:: https://circleci.com/gh/commontk/CTK.png?style=shield
    :target: https://circleci.com/gh/commontk/CTK

The Common Toolkit is a community effort to provide support code for medical image analysis,
surgical navigation, and related projects.

See http://commontk.org

Build Instructions
==================

Configure the project using CMake.

For Qt5, specify the followings:
  - ``CTK_QT_VERSION``: 5
  - ``QT5_DIR``: C:\Qt\5.15.0\msvc2019_64\lib\cmake\Qt5 (or something similar, depending on operating system)
  - ``VTK_MODULE_ENABLE_VTK_GUISupportQt``: YES (for enabling VTK widgets)
  - ``VTK_MODULE_ENABLE_VTK_ViewsQt``: YES (for enabling VTK view widgets)

Note: make sure your built toolchain version is compatible with the chosen Qt version. For example if trying to build with Qt-5.12 and Microsoft Visual Studio 2019, then build will fail with the error `error LNK2019: unresolved external symbol "__declspec(dllimport) public: __cdecl QLinkedListData::QLinkedListData(void)"`. The solution is to either change the toolset version to an earlier one (e.g., Visual Studio 2017) or upgrade Qt (e.g., use Qt-5.15 with Visual Studio 2019).
