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
  ctkDICOMEchoTest1.cpp
  ctkDICOMItemTest1.cpp
  ctkDICOMIndexerTest1.cpp
  ctkDICOMJobTest1.cpp
  ctkDICOMJobResponseSetTest1.cpp
  ctkDICOMModelTest1.cpp
  ctkDICOMPersonNameTest1.cpp
  ctkDICOMQueryTest1.cpp
  ctkDICOMQueryTest2.cpp
  ctkDICOMRetrieveTest1.cpp
  ctkDICOMRetrieveTest2.cpp
  ctkDICOMSchedulerTest1.cpp
  ctkDICOMServerTest1.cpp
  ctkDICOMTesterTest1.cpp
  ctkDICOMTesterTest2.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

set(LIBRARY_NAME ${PROJECT_NAME})

ctk_add_executable_utf8(${KIT}CppTests ${Tests})
target_link_libraries(${KIT}CppTests ${LIBRARY_NAME})

find_package(Qt${CTK_QT_VERSION} COMPONENTS Test Widgets REQUIRED)
target_link_libraries(${KIT}CppTests
  Qt${CTK_QT_VERSION}::Test
  Qt${CTK_QT_VERSION}::Widgets
  )

#
# Add Tests
#

SIMPLE_TEST(ctkDICOMJobTest1)
SIMPLE_TEST(ctkDICOMJobResponseSetTest1)
SIMPLE_TEST(ctkDICOMServerTest1)

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

# ctkDICOMEcho
SIMPLE_TEST(ctkDICOMEchoTest1
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )

# ctkDICOMModel
SIMPLE_TEST(ctkDICOMModelTest1
  ${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary/ctkDICOMModelTest1-dicom.db
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql
  )
SIMPLE_TEST(ctkDICOMPersonNameTest1)

# ctkDICOMQuery
SIMPLE_TEST( ctkDICOMQueryTest1)
SIMPLE_TEST( ctkDICOMQueryTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )
set_property(TEST "ctkDICOMQueryTest2" PROPERTY RESOURCE_LOCK "dcmqrscp")

# ctkDICOMRetrieve
SIMPLE_TEST( ctkDICOMRetrieveTest1)
SIMPLE_TEST( ctkDICOMRetrieveTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )
set_property(TEST "ctkDICOMRetrieveTest2" PROPERTY RESOURCE_LOCK "dcmqrscp")

# ctkDICOMCore
SIMPLE_TEST( ctkDICOMCoreTest1
  ${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary/ctkDICOMCoreTest1-dicom.db
  ${CMAKE_CURRENT_SOURCE_DIR}/../../Resources/dicom-sample.sql
  )

# ctkDICOMScheduler
SIMPLE_TEST(ctkDICOMSchedulerTest1
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000050.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000051.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000052.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000053.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000054.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000057.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000058.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000059.IMA
  )

# ctkDICOMTester
SIMPLE_TEST( ctkDICOMTesterTest1 )
set_property(TEST "ctkDICOMTesterTest1" PROPERTY RESOURCE_LOCK "dcmqrscp")
SIMPLE_TEST( ctkDICOMTesterTest2
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA
  ${CTKData_DIR}/Data/DICOM/MRHEAD/000056.IMA
  )
set_property(TEST "ctkDICOMTesterTest2" PROPERTY RESOURCE_LOCK "dcmqrscp")
