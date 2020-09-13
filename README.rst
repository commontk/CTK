Common Toolkit
==============

.. image:: https://circleci.com/gh/commontk/CTK.png?style=shield
    :target: https://circleci.com/gh/commontk/CTK

The Common Toolkit is a community effort to provide support code for medical image analysis,
surgical navigation, and related projects.

See http://commontk.org



Build Instructions
==================

Note when trying to build with Microsoft Visual Studio 2019. It seems due to a bug either in Qt5 or MSVC2019 Platform Toolset the compilation of the PluginFramework triggers an error. After having the intuition to change the toolset version to a lower version it compiled without any errors. The rest of your generated VS solution can use the default Platform Toolset set to MSVC 2019, only the PluginFramework has to be set to a lower version.

The temporary issue/solution at the moment is discussed at the following places:

https://github.com/commontk/CTK/issues/929
https://forum.qt.io/topic/117230/qlinkedlist-undefined-external-symbol
