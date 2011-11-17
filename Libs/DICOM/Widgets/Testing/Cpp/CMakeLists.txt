set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMAppWidgetTest1.cpp
  ctkDICOMDatasetViewTest1.cpp
  ctkDICOMDirectoryListWidgetTest1.cpp
  ctkDICOMImageTest1.cpp
  ctkDICOMImportWidgetTest1.cpp
  ctkDICOMListenerWidgetTest1.cpp
  ctkDICOMModelTest2.cpp
  ctkDICOMQueryResultsTabWidgetTest1.cpp
  ctkDICOMQueryRetrieveWidgetTest1.cpp
  ctkDICOMServerNodeWidgetTest1.cpp
  ctkDICOMThumbnailListWidgetTest1.cpp
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

SIMPLE_TEST(ctkDICOMAppWidgetTest1)
add_test( ctkDICOMDatasetViewTest1  ${KIT_TESTS} 
          ctkDICOMDatasetViewTest1  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
set_property(TEST ctkDICOMDatasetViewTest1 PROPERTY LABELS ${PROJECT_NAME})
SIMPLE_TEST(ctkDICOMDirectoryListWidgetTest1)

add_test( ctkDICOMImageTest1 ${KIT_TESTS}
          ctkDICOMImageTest1 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
set_property(TEST ctkDICOMImageTest1 PROPERTY LABELS ${PROJECT_NAME})

SIMPLE_TEST(ctkDICOMImportWidgetTest1)
SIMPLE_TEST(ctkDICOMListenerWidgetTest1)

add_test( ctkDICOMModelTest2 ${KIT_TESTS}
          ctkDICOMModelTest2 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                             ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Resources/dicom-sample.sql)
set_property(TEST ctkDICOMModelTest2 PROPERTY LABELS ${PROJECT_NAME})

SIMPLE_TEST(ctkDICOMQueryRetrieveWidgetTest1)
SIMPLE_TEST(ctkDICOMQueryResultsTabWidgetTest1)
SIMPLE_TEST(ctkDICOMThumbnailListWidgetTest1)
