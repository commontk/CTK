#
# See CMake/ctkMacroGetTargetLibraries.cmake
# 
# This file should list the libraries required to build the current CTK application.
# 

SET(plugin_dependencies
  org_commontk_plugingenerator
)

SET(target_libraries
  CTKPluginFramework
  plugin_dependencies
  )
