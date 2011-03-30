SET(KIT ${PROJECT_NAME})

CREATE_TEST_SOURCELIST(Tests ${KIT}CppTests.cpp
  ctkDICOMAppWidgetTest1.cpp
  ctkDICOMDatasetViewTest1.cpp
  ctkDICOMDirectoryListWidgetTest1.cpp
  ctkDICOMImportWidgetTest1.cpp
  ctkDICOMListenerWidgetTest1.cpp
  ctkDICOMModelTest2.cpp
  ctkDICOMQueryRetrieveWidgetTest1.cpp
  ctkDICOMQueryResultsTabWidgetTest1.cpp
  ctkDICOMServerNodeWidgetTest1.cpp
  ctkDICOMThumbnailWidgetTest1.cpp
  )

SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CppTests.cpp)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CppTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CppTests ${LIBRARY_NAME})

SET( KIT_TESTS ${CPP_TEST_PATH}/${KIT}CppTests)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${KIT_TESTS} ${TESTNAME} )
  SET_PROPERTY(TEST ${TESTNAME} PROPERTY LABELS ${PROJECT_NAME})
ENDMACRO( SIMPLE_TEST  )

#
# Add Tests
#

SIMPLE_TEST(ctkDICOMAppWidgetTest1)
ADD_TEST( ctkDICOMDatasetViewTest1  ${KIT_TESTS} 
          ctkDICOMDatasetViewTest1  ${CTKData_DIR}/Data/DICOM/MRHEAD/000055.IMA)
SET_PROPERTY(TEST ctkDICOMDatasetViewTest1 PROPERTY LABELS ${PROJECT_NAME})
SIMPLE_TEST(ctkDICOMDirectoryListWidgetTest1)
SIMPLE_TEST(ctkDICOMImportWidgetTest1)
SIMPLE_TEST(ctkDICOMListenerWidgetTest1)

ADD_TEST( ctkDICOMModelTest2 ${KIT_TESTS}
          ctkDICOMModelTest2 ${CMAKE_CURRENT_BINARY_DIR}/dicom.db
                             ${CMAKE_CURRENT_SOURCE_DIR}/../../../Core/Resources/dicom-sample.sql)
SET_PROPERTY(TEST ctkDICOMModelTest2 PROPERTY LABELS ${PROJECT_NAME})

SIMPLE_TEST(ctkDICOMQueryRetrieveWidgetTest1)
SIMPLE_TEST(ctkDICOMQueryResultsTabWidgetTest1)
SIMPLE_TEST(ctkDICOMThumbnailWidgetTest1)
