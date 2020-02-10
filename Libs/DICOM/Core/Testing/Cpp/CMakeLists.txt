set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMCoreTest1.cpp
  ctkDICOMDatabaseTest1.cpp
  ctkDICOMDatabaseTest2.cpp
  ctkDICOMDatabaseTest3.cpp
  ctkDICOMDatabaseTest4.cpp
  ctkDICOMDatabaseTest5.cpp
  ctkDICOMDatabaseTest6.cpp
  ctkDICOMDatabaseTest7.cpp
  ctkDICOMItemTest1.cpp
  ctkDICOMIndexerTest1.cpp
  ctkDICOMModelTest1.cpp
  ctkDICOMPersonNameTest1.cpp
  ctkDICOMQueryTest1.cpp
  ctkDICOMQueryTest2.cpp
  ctkDICOMRetrieveTest1.cpp
  ctkDICOMRetrieveTest2.cpp
  ctkDICOMTesterTest1.cpp
  ctkDICOMTesterTest2.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

#
# Add Tests
#

# ctkDICOMDatabase
SIMPLE_TEST(ctkDICOMDatabaseTest1)
SIMPLE_TEST(ctkDICOMDatabaseTest2 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMDatabaseTest3
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-unversioned-schema.sql
  )
SIMPLE_TEST(ctkDICOMDatabaseTest4 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMDatabaseTest5 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMDatabaseTest6 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMDatabaseTest7)
SIMPLE_TEST(ctkDICOMItemTest1)
SIMPLE_TEST(ctkDICOMIndexerTest1 )

# ctkDICOMModel
SIMPLE_TEST(ctkDICOMModelTest1
  ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql
  )
SIMPLE_TEST(ctkDICOMPersonNameTest1)

# ctkDICOMQuery
SIMPLE_TEST( ctkDICOMQueryTest1)
SIMPLE_TEST( ctkDICOMQueryTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )

# ctkDICOMRetrieve
SIMPLE_TEST( ctkDICOMRetrieveTest1)
SIMPLE_TEST( ctkDICOMRetrieveTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )

# ctkDICOMCore
SIMPLE_TEST( ctkDICOMCoreTest1
  ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql
  )

# ctkDICOMTester
SIMPLE_TEST( ctkDICOMTesterTest1 )
SIMPLE_TEST( ctkDICOMTesterTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )
