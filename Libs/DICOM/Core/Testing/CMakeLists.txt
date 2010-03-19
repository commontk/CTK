SET(KIT CTKDICOMCore)

CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  ctkDICOMModelTest1.cxx
  ctkDICOMTest1.cxx
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests ${LIBRARY_NAME})

SET( CTKDICOMCore_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)
IF(WIN32)
  SET(CTKDICOMCore_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CxxTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

ADD_TEST( ctkDICOMModelTest1 ${CTKDICOMCore_TESTS}
          ctkDICOMModelTest1 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                             ${CMAKE_CURRENT_SOURCE_DIR}/../Resources/dicom-sample.sql)

ADD_TEST( ctkDICOMTest1 ${CTKDICOMCore_TESTS}
          ctkDICOMTest1 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                        ${CMAKE_CURRENT_SOURCE_DIR}/../Resources/dicom-sample.sql)
