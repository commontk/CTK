set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMDatabaseTest1.cpp
  ctkDICOMDatasetTest1.cpp
  ctkDICOMIndexerTest1.cpp
  ctkDICOMModelTest1.cpp
  ctkDICOMPersonNameTest1.cpp
  ctkDICOMQueryTest1.cpp
  ctkDICOMQueryTest2.cpp
  ctkDICOMRetrieveTest1.cpp
  ctkDICOMRetrieveTest2.cpp
  ctkDICOMTest1.cpp
  ctkDICOMTesterTest1.cpp
  ctkDICOMTesterTest2.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

add_executable(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

set( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

macro( SIMPLE_TEST  TESTNAME )
  add_test( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  set_property(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
endmacro( SIMPLE_TEST  )

#
# Add Tests
#
SIMPLE_TEST(ctkDICOMDatabaseTest1)
SIMPLE_TEST(ctkDICOMDatasetTest1)
SIMPLE_TEST(ctkDICOMIndexerTest1 )

add_test( ctkDICOMModelTest1 ${KIT_TESTS}
          ctkDICOMModelTest1 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                             ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql)
set_property(TEST ctkDICOMModelTest1 PROPERTY LABELS ${PROJECT_NAME})
SIMPLE_TEST(ctkDICOMPersonNameTest1)

# ctkDICOMQuery
SIMPLE_TEST( ctkDICOMQueryTest1)
add_test( ctkDICOMQueryTest2 ${KIT_TESTS}
          ctkDICOMQueryTest2 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA)
set_property(TEST ctkDICOMQueryTest2 PROPERTY LABELS ${PROJECT_NAME})

# ctkDICOMRetrieve
SIMPLE_TEST( ctkDICOMRetrieveTest1)
add_test( ctkDICOMRetrieveTest2 ${KIT_TESTS}
          ctkDICOMRetrieveTest2 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA)
set_property(TEST ctkDICOMRetrieveTest2 PROPERTY LABELS ${PROJECT_NAME})

# ctkDICOMTest
add_test( ctkDICOMTest1 ${KIT_TESTS}
          ctkDICOMTest1 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                        ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql)
set_property(TEST ctkDICOMTest1 PROPERTY LABELS ${PROJECT_NAME})

# ctkDICOMTester
SIMPLE_TEST(ctkDICOMTesterTest1)
add_test( ctkDICOMTesterTest2 ${KIT_TESTS}
          ctkDICOMTesterTest2 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA)
set_property(TEST ctkDICOMTesterTest2 PROPERTY LABELS ${PROJECT_NAME})
