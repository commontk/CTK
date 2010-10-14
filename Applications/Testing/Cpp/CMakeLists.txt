SET(KIT CTKApplication)

SET(KIT_TEST_SRCS)

## Query / Retrieve Testing
if(CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)
  LIST(APPEND KIT_TEST_SRCS
    ctkDICOMApplicationTest1.cpp
    )
endif()

# If there is no tests, exit
LIST(LENGTH KIT_TEST_SRCS KIT_TEST_SRCS_LENGTH)
if(${KIT_TEST_SRCS_LENGTH} EQUAL 0)
  RETURN()
endif()

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ${KIT_TEST_SRCS}
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

# Target libraries - See CMake/ctkMacroGetTargetLibraries.cmake
# The following macro will read the target libraries from the file 'target_libraries.cmake'
ctkMacroGetTargetLibraries(KIT_target_libraries)

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${KIT_target_libraries})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )


## Query / Retrieve Testing
if (CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)

  # create a dcm query/retrieve service config file that points to the build dir
  set (DCMQRSCP_STORE_DIR ${CTK_BINARY_DIR}/Testing/Temporary)
  set (ctkDICOMRetrieve_STORE_DIR ${CTK_BINARY_DIR}/Testing/Temporary/ctkDICOMRetrieveStorage)
  set (DCMQRSCP_CONFIG ${CTK_BINARY_DIR}/Testing/Temporary/dcmqrscp.cfg)
  CONFIGURE_FILE( dcmqrscp.cfg.in ${DCMQRSCP_CONFIG} )

  set(CTK_APPLICATION_RUNTIME_DIRECTORY ${CTK_CMAKE_RUNTIME_OUTPUT_DIRECTORY})
  if(WIN32)
    set(CTK_APPLICATION_RUNTIME_DIRECTORY
          ${CTK_APPLICATION_RUNTIME_DIRECTORY}/${CMAKE_BUILD_TYPE})
  endif(WIN32)

  set(ctkDICOMQuery_EXECUTABLE ${CTK_APPLICATION_RUNTIME_DIRECTORY}/ctkDICOMQuery)
  set(ctkDICOMQuery_DB_FILE ${CTK_BINARY_DIR}/Testing/Temporary/ctkDICOMApplicationTest1.db)
  set(ctkDICOMRetrieve_EXECUTABLE ${CTK_APPLICATION_RUNTIME_DIRECTORY}/ctkDICOMRetrieve)

  #
  # Add Tests
  #

  ADD_TEST( ctkDICOMApplicationTest1 ${KIT_TESTS}
            ctkDICOMApplicationTest1
                 ${DCMTK_DCMQRSCP_EXECUTABLE}
                 ${DCMQRSCP_CONFIG}
                 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
                 ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
                 ${DCMTK_STORESCU_EXECUTABLE}
                 ${ctkDICOMQuery_EXECUTABLE}
                 ${ctkDICOMQuery_DB_FILE}
                 ${ctkDICOMRetrieve_EXECUTABLE}
                 ${ctkDICOMRetrieve_STORE_DIR}
                 )
  SET_PROPERTY(TEST ctkDICOMApplicationTest1 PROPERTY LABELS ${PROJECT_NAME})
endif (CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)
