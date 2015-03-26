###########################################################################
#
#  Library:   CTK
#
#  Copyright (c) Kitware Inc.
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0.txt
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
###########################################################################

#
# This module is provided as CTK_USE_FILE by CTKConfig.cmake. It can
# be INCLUDED in a project to load the needed compiler and linker
# settings to use CTK.
#

if(NOT CTK_USE_FILE_INCLUDED)
  set(CTK_USE_FILE_INCLUDED 1)

  # Add include directories needed to use CTK.
  get_directory_property(existing_include_directories INCLUDE_DIRECTORIES)
  set(new_include_directories ${CTK_INCLUDE_DIRS})
  if(existing_include_directories)
    list(REMOVE_ITEM new_include_directories ${existing_include_directories})
  endif()
  include_directories(${new_include_directories})

  if (NOT DEFINED QT_QMAKE_EXECUTABLE)
    set(QT_QMAKE_EXECUTABLE ${CTK_QT_QMAKE_EXECUTABLE})
  endif()

endif()
