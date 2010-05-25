This directory contains a stripped QtMobility 1.0.0 tar ball containing
only the service framework sources. The following files have been modified:

- qtmobility.pro (exclude the plugins directory)
- configure.bat (skip compile tests)

On Windows, Perl would be needed for a working install step (for
generating the header file wrappers). To avoid having another
dependency on an external tool, everything is installed in a
common directory (prefix). The lib files are then copied to the
CTK binary dir and the header file wrappers are copied by cmake
into the install dir.

