set(KIT ${PROJECT_NAME})

create_test_sourcelist(Tests ${KIT}CppTests.cpp
  ctkDICOMAppWidgetTest1.cpp
  ctkDICOMBrowserTest1.cpp
  ctkDICOMItemViewTest1.cpp
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

#
# Add Tests
#

SIMPLE_TEST(ctkDICOMAppWidgetTest1 ${CTKData_DIR}/Data/DICOM/MRHEAD)
SIMPLE_TEST(ctkDICOMBrowserTest1 ${CTKData_DIR}/Data/DICOM/MRHEAD)
SIMPLE_TEST(ctkDICOMItemViewTest1 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMDirectoryListWidgetTest1)
SIMPLE_TEST(ctkDICOMImageTest1 ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SIMPLE_TEST(ctkDICOMImportWidgetTest1)
SIMPLE_TEST(ctkDICOMListenerWidgetTest1)
SIMPLE_TEST(ctkDICOMModelTest2
  ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
  ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Resources/dicom-sample.sql
  )
SIMPLE_TEST(ctkDICOMQueryRetrieveWidgetTest1)
SIMPLE_TEST(ctkDICOMQueryResultsTabWidgetTest1)
SIMPLE_TEST(ctkDICOMThumbnailListWidgetTest1 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Resources/dicom-sample.sql)
