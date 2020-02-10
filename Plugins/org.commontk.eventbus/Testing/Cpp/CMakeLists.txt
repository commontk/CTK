macro(ctkMacroInitProject test)

  # Extract current directory name to use as project name
  #file(GLOB CUR_FILE "CMakeLists.txt")
  #get_filename_component(CUR_ABSOLUTE_DIR ${CUR_FILE} PATH)
  #get_filename_component(DIR_NAME ${CUR_ABSOLUTE_DIR} NAME)
  #project(${DIR_NAME})

  file(GLOB include_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  file(GLOB implementation_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
  file(GLOB templete_file_list1 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
  file(GLOB templete_file_list2 "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
  file(GLOB resource_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
  file(GLOB ui_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

  # Set your list of sources here.
  set(PROJECT_SRCS
        ${implementation_file_list}
        ${include_file_list}
        ${templete_file_list1}
        ${templete_file_list2}
        ${ui_file_list}
        ${resource_file_list}
  )

  ## Add the project binary dir as include dir for the .moc files.
  include_directories("${CMAKE_CURRENT_BINARY_DIR}")

  set(MY_MOC_CXX )
  if(${test})
    ## Moc the tests:
    foreach(FILE_NAME_ABS ${implementation_file_list})
      ## extract the base file name.
      get_filename_component(FILE_NAME ${FILE_NAME_ABS} NAME_WE)
      ## Exclude the main.cpp file (it doesn't ned to be 'mocced')
      if(NOT ${FILE_NAME} STREQUAL "main")
        ## Assign the moc custom filename
        set(MOC_FILE "${FILE_NAME}.moc")
        if (CTK_QT_VERSION VERSION_GREATER "4")
          QT5_GENERATE_MOC(${FILE_NAME_ABS} ${MOC_FILE})
        else()
          QT4_GENERATE_MOC(${FILE_NAME_ABS} ${MOC_FILE})
        endif()
        list(APPEND MY_MOC_CXX "${CMAKE_CURRENT_BINARY_DIR}/${MOC_FILE}")
      endif()
    endforeach()
  else()
    ## Moc the library's .h files
    if (CTK_QT_VERSION VERSION_GREATER "4")
      QT5_WRAP_CPP(MY_MOC_CXX ${include_file_list})
      QT5_WRAP_UI(MY_UI_CXX ${ui_file_list})
      #QT5_ADD_RESOURCES(MY_RESOURCE_CXX ${resource_file_list})
    else()
      QT4_WRAP_CPP(MY_MOC_CXX ${include_file_list})
      QT4_WRAP_UI(MY_UI_CXX ${ui_file_list})
      #QT4_ADD_RESOURCES(MY_RESOURCE_CXX ${resource_file_list})
    endif()
  endif()

  set(PROJECT_SRCS
    ${PROJECT_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    ${resource_file_list}
    )

  # List libraries that are needed by this project.
  ctkFunctionGetTargetLibraries(dependency_libraries)
  set(PROJECT_LIBS ${dependency_libraries})

endmacro()

set(test_executable ${PROJECT_NAME}CppTests)

ctkMacroInitproject(1)

ctk_add_executable_utf8(${test_executable} ${PROJECT_SRCS})
target_link_libraries(${test_executable} ${PROJECT_LIBS})

add_test(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})

#link_libraries(${PROJECT_LIBS})
