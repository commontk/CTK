set(KIT CTKApplication)

set(KIT_TEST_SRCS)
set(KIT_target_libraries)

## Query / Retrieve Testing
if(CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)
  list(APPEND KIT_TEST_SRCS
    ctkDICOMApplicationTest1.cpp
    )
  ctkFunctionGetTargetLibraries(KIT_target_libraries ${ctkDICOMQuery_SOURCE_DIR})
  ctkFunctionGetTargetLibraries(KIT_target_libraries ${ctkDICOMRetrieve_SOURCE_DIR})
endif()

if(KIT_TEST_SRCS)

  create_test_sourcelist(Tests ${KIT}CppTests.cpp
    ${KIT_TEST_SRCS}
    )

  SET (TestsToRun ${Tests})
  REMOVE (TestsToRun ${KIT}CppTests.cpp)

  add_executable(${KIT}CppTests ${Tests})
  if(KIT_target_libraries)
    target_link_libraries(${KIT}CppTests ${KIT_target_libraries})
  endif()

  ## Query / Retrieve Testing
  if(CTK_APP_ctkDICOMQuery AND CTK_APP_ctkDICOMRetrieve)

    #
    # create a dcm query/retrieve service config file that points to the build dir
    #
    set(DCMQRSCP_STORE_DIR ${CTK_BINARY_DIR}/Testing/Temporary)
    set(ctkDICOMRetrieve_STORE_DIR ${CTK_BINARY_DIR}/Testing/Temporary/ctkDICOMRetrieveStorage)
    set(DCMQRSCP_CONFIG ${CTK_BINARY_DIR}/Testing/Temporary/dcmqrscp.cfg)
    configure_file( dcmqrscp.cfg.in ${DCMQRSCP_CONFIG} )

    set(ctkDICOMQuery_DB_FILE ${CTK_BINARY_DIR}/Testing/Temporary/ctkDICOMApplicationTest1.db)

    #
    # find the helper executables
    #
    find_program(DCMTK_DCMQRSCP_EXECUTABLE dcmqrscp
      PATHS
        ${DCMTK_DIR}/bin
      NO_DEFAULT_PATH
      )

    find_program(DCMTK_STORESCU_EXECUTABLE storescu
      PATHS
        ${DCMTK_DIR}/bin
      NO_DEFAULT_PATH
      )

    #
    # Add Tests
    #

    SIMPLE_TEST( ctkDICOMApplicationTest1
                 ${DCMTK_DCMQRSCP_EXECUTABLE}
                 ${DCMQRSCP_CONFIG}
                 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
                 ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
                 ${DCMTK_STORESCU_EXECUTABLE}
                 $<TARGET_FILE:ctkDICOMQuery>
                 ${ctkDICOMQuery_DB_FILE}
                 $<TARGET_FILE:ctkDICOMRetrieve>
                 ${ctkDICOMRetrieve_STORE_DIR}
                 )
  endif()

endif()

