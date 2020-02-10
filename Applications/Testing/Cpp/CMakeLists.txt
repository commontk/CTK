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
  ctk_add_executable_utf8(ctkDICOMApplicationTest1 ctkDICOMApplicationTest1.cpp)
  if(CTK_QT_VERSION VERSION_LESS "5")
    target_link_libraries(ctkDICOMApplicationTest1 ${QT_LIBRARIES})
  else()
    target_link_libraries(ctkDICOMApplicationTest1 Qt5::Core)
  endif()

  ADD_TEST( NAME ctkDICOMApplicationTest1
            COMMAND
            $<TARGET_FILE:ctkDICOMApplicationTest1>
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
