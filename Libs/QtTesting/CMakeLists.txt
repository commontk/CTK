project(CTKQtTesting)

#
# 3rd party dependencies
#
find_package(QtTesting REQUIRED)
set(QtTesting_LIBRARIES QtTesting)

#
# See CTK/CMake/ctkMacroBuildLib.cmake for details
#

set(KIT_export_directive "CTK_QTTESTING_EXPORT")

set(CTK_LIBS_WIDGET_DIR)

# TODO The following code used to create CTK_ENABLED_LIBNAMES list could be
#      moved in the main CMakeLists.txt
set(CTK_ENABLED_LIBNAMES)
foreach(enabled_lib ${CTK_ENABLED_LIBS})
  string(REPLACE "/" "" lib_name_no_slash ${enabled_lib})
  list(APPEND CTK_ENABLED_LIBNAMES CTK${lib_name_no_slash})
  string(REGEX MATCHALL "Widgets" CTK_LIB_WIDGET ${lib_name_no_slash})
  if(CTK_LIB_WIDGET)
    list(APPEND CTK_LIBS_WIDGET_DIR CTK${lib_name_no_slash})
  endif()
endforeach()

# Recover All the event player/translator headers
set(CTK_HEADERS_EVENT_PLAYERS)
set(CTK_HEADERS_EVENT_TRANSLATORS)

foreach(CTK_LIB_WIDGET_DIR ${CTK_LIBS_WIDGET_DIR})
  file(GLOB HEADERS RELATIVE ${${CTK_LIB_WIDGET_DIR}_SOURCE_DIR} "${${CTK_LIB_WIDGET_DIR}_SOURCE_DIR}/*.h")
  foreach(HEADER ${HEADERS})
    string(REGEX MATCHALL "EventPlayer" HEADER_PLAYER ${HEADER})
    string(REGEX MATCHALL "EventTranslator" HEADER_TRANSLATOR ${HEADER})
    if(HEADER_PLAYER)
      list(APPEND CTK_HEADERS_EVENT_PLAYERS ${HEADER})
    endif()
    if(HEADER_TRANSLATOR)
      list(APPEND CTK_HEADERS_EVENT_TRANSLATORS ${HEADER})
    endif()
  endforeach()
endforeach()

if(CTK_HEADERS_EVENT_PLAYERS)
  list(SORT CTK_HEADERS_EVENT_PLAYERS)
endif()
if(CTK_HEADERS_EVENT_TRANSLATORS)
  list(SORT CTK_HEADERS_EVENT_TRANSLATORS)
endif()

# Define the C++ code for the file ctkQtTestingUtility
# Define the headers
# Define the methode addWidgetEventTranslator/Player()
set(CTK_ADD_HEADER_EVENT_PLAYERS "// Event player includes\n")
#set(CTK_ADD_WIDGET_EVENT_PLAYERS)
foreach(CTK_HEADER_EVENT_PLAYERS ${CTK_HEADERS_EVENT_PLAYERS})
  set(CTK_ADD_HEADER_EVENT_PLAYERS "${CTK_ADD_HEADER_EVENT_PLAYERS}#include <${CTK_HEADER_EVENT_PLAYERS}>\n")
  get_filename_component(CTK_HEADER_EVENT_PLAYER_NAME ${CTK_HEADER_EVENT_PLAYERS} NAME_WE)

  set(is_special_player FALSE)

  string(COMPARE EQUAL "ctkVTKRenderViewEventPlayer" ${CTK_HEADER_EVENT_PLAYER_NAME} is_special_player)
  if(is_special_player)
    set(CTK_ADD_WIDGET_EVENT_PLAYERS "${CTK_ADD_WIDGET_EVENT_PLAYERS}  this->eventPlayer()->addWidgetEventPlayer(new ${CTK_HEADER_EVENT_PLAYER_NAME}(\"QVTKWidget\"));\n")
  endif()

  if(NOT is_special_player)
    # Explicitly skip this player so that it could be added first in the list of players to register.
    string(COMPARE EQUAL "ctkFileDialogEventPlayer" ${CTK_HEADER_EVENT_PLAYER_NAME} is_special_player)
  endif()

  if(NOT is_special_player)
    set(CTK_ADD_WIDGET_EVENT_PLAYERS "${CTK_ADD_WIDGET_EVENT_PLAYERS}  this->eventPlayer()->addWidgetEventPlayer(new ${CTK_HEADER_EVENT_PLAYER_NAME}());\n")
  endif()
endforeach()

if(CTK_LIB_Widgets)
set(CTK_ADD_WIDGET_EVENT_PLAYERS "${CTK_ADD_WIDGET_EVENT_PLAYERS}  this->eventPlayer()->addWidgetEventPlayer(new ctkFileDialogEventPlayer(util));\n")
endif()

set(CTK_ADD_HEADER_EVENT_TRANSLATORS "// Event translator includes\n")
#set(CTK_ADD_WIDGET_EVENT_TRANSLATORS)
foreach(CTK_HEADER_EVENT_TRANSLATORS ${CTK_HEADERS_EVENT_TRANSLATORS})
  set(CTK_ADD_HEADER_EVENT_TRANSLATORS "${CTK_ADD_HEADER_EVENT_TRANSLATORS}#include <${CTK_HEADER_EVENT_TRANSLATORS}>\n")
  get_filename_component(CTK_HEADER_EVENT_TRANSLATOR_NAME ${CTK_HEADER_EVENT_TRANSLATORS} NAME_WE)

  set(is_special_translator FALSE)

  string(COMPARE EQUAL "ctkVTKRenderViewEventTranslator" ${CTK_HEADER_EVENT_TRANSLATOR_NAME} is_special_translator)
  if(is_special_translator)
    set(CTK_ADD_WIDGET_EVENT_TRANSLATORS "${CTK_ADD_WIDGET_EVENT_TRANSLATORS}  this->eventTranslator()->addWidgetEventTranslator(new ${CTK_HEADER_EVENT_TRANSLATOR_NAME}(\"QVTKWidget\"));\n")
  endif()

  if(NOT is_special_translator)
    # Explicitly skip this translator so that it could be added first in the list of translators to register.
    string(COMPARE EQUAL "ctkFileDialogEventTranslator" ${CTK_HEADER_EVENT_TRANSLATOR_NAME} is_special_translator)
  endif()

  if(NOT is_special_translator)
    set(CTK_ADD_WIDGET_EVENT_TRANSLATORS "${CTK_ADD_WIDGET_EVENT_TRANSLATORS}  this->eventTranslator()->addWidgetEventTranslator(new ${CTK_HEADER_EVENT_TRANSLATOR_NAME}());\n")
  endif()
endforeach()

if (CTK_LIB_Widgets)
  set(CTK_ADD_WIDGET_EVENT_TRANSLATORS "${CTK_ADD_WIDGET_EVENT_TRANSLATORS}  this->eventTranslator()->addWidgetEventTranslator(new ctkFileDialogEventTranslator(util));\n")
endif()

configure_file(
  ctkQtTestingUtility.cpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/ctkQtTestingUtility.cpp
  )

# Source files
set(KIT_SRCS
  ctkEventTranslatorPlayerWidget.cpp
  ctkEventTranslatorPlayerWidget.h
  ${CMAKE_CURRENT_BINARY_DIR}/ctkQtTestingUtility.cpp
  ctkQtTestingUtility.h
  ctkXMLEventObserver.cpp
  ctkXMLEventObserver.h
  ctkXMLEventSource.cpp
  ctkXMLEventSource.h
  )

# Header that should run through moc
set(KIT_MOC_SRCS
  ctkEventTranslatorPlayerWidget.h
  ctkQtTestingUtility.h
  ctkXMLEventObserver.h
  ctkXMLEventSource.h
  )

# UI files
set(KIT_UI_FORMS
  Resources/UI/ctkEventTranslatorPlayerWidget.ui
)

# Resources
set(KIT_resources
  Resources/ctkQtTesting.qrc
)

# Set QtTesting_LIBRARIES variable
set(QtTesting_LIBRARIES QtTesting)

# Target libraries - See CMake/ctkFunctionGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkFunctionGetTargetLibraries(KIT_target_libraries)

if(CTK_QT_VERSION VERSION_GREATER "4")
  list(APPEND KIT_target_libraries Qt5::Xml Qt5::XmlPatterns)
endif()

ctkMacroBuildLib(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${KIT_export_directive}
  SRCS ${KIT_SRCS}
  MOC_SRCS ${KIT_MOC_SRCS}
  UI_FORMS ${KIT_UI_FORMS}
  TARGET_LIBRARIES ${KIT_target_libraries}
  RESOURCES ${KIT_resources}
  LIBRARY_TYPE ${CTK_LIBRARY_MODE}
  )

if(CTK_WRAP_PYTHONQT_LIGHT)
  ctkMacroBuildLibWrapper(
    TARGET ${PROJECT_NAME}
    SRCS ${KIT_SRCS}
    WRAPPER_LIBRARY_TYPE ${CTK_LIBRARY_MODE}
    )
endif()

# Testing
#if(BUILD_TESTING)
#  add_subdirectory(Testing)
#endif()

