MACRO(ctkMacroInitProject test)

  # Extract current directory name to use as project name
  #file(GLOB CUR_FILE "CMakeLists.txt")
  #get_filename_component(CUR_ABSOLUTE_DIR ${CUR_FILE} PATH)
  #get_filename_component(DIR_NAME ${CUR_ABSOLUTE_DIR} NAME)
  #PROJECT(${DIR_NAME})

  FILE(GLOB include_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
  FILE(GLOB implementation_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
  FILE(GLOB templete_file_list1 "${CMAKE_CURRENT_SOURCE_DIR}/*.txx")
  FILE(GLOB templete_file_list2 "${CMAKE_CURRENT_SOURCE_DIR}/*.tpp")
  FILE(GLOB resource_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.qrc")
  FILE(GLOB ui_file_list "${CMAKE_CURRENT_SOURCE_DIR}/*.ui")

  # Set your list of sources here.
  SET(PROJECT_SRCS
        ${implementation_file_list}
        ${include_file_list}
        ${templete_file_list1}
        ${templete_file_list2}
        ${ui_file_list}
        ${resource_file_list}
  )

  ## Add the project binary dir as include dir for the .moc files.
  INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

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
        QT4_GENERATE_MOC(${FILE_NAME_ABS} ${MOC_FILE})
        LIST(APPEND MY_MOC_CXX "${CMAKE_CURRENT_BINARY_DIR}/${MOC_FILE}")
      endif(NOT ${FILE_NAME} STREQUAL "main")
    endforeach()
  else(${test})
    ## Moc the library's .h files
    QT4_WRAP_CPP(MY_MOC_CXX ${include_file_list})
    QT4_WRAP_UI(MY_UI_CXX ${ui_file_list})
    #QT4_ADD_RESOURCES(MY_RESOURCE_CXX ${resource_file_list})
  endif(${test})

  SET(PROJECT_SRCS
    ${PROJECT_SRCS}
    ${MY_MOC_CXX}
    ${MY_UI_CXX}
    ${resource_file_list}
    )

  # List libraries that are needed by this project.
  ctkFunctionGetTargetLibraries(dependency_libraries)
  SET(PROJECT_LIBS ${dependency_libraries})

ENDMACRO()

SET(test_executable ${PROJECT_NAME}CppTests)

ctkMacroInitProject(1)

ADD_EXECUTABLE(${test_executable} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${test_executable} ${PROJECT_LIBS})

ADD_TEST(${PROJECT_NAME}Tests ${CPP_TEST_PATH}/${test_executable})

#LINK_LIBRARIES(${PROJECT_LIBS})
