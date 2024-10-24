/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Davide Punzo, punzodavide@hotmail.it,
  and development was supported by the Center for Intelligent Image-guided Interventions (CI3).

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QString>
#include <QStringList>
#include <QTimer>

// ctkCore includes
#include <ctkCoreTestingMacros.h>

// ctkDICOMWidget includes
#include "ctkDICOMPatientItemWidget.h"

// Test visual browser import functionality
int ctkDICOMPatientItemWidgetTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);

  bool interactive = arguments.removeOne("-I");

  ctkDICOMPatientItemWidget widget;

  // Test the default values
  CHECK_QSTRING(widget.patientItem(), "");
  CHECK_QSTRING(widget.patientID(), "");
  CHECK_QSTRING(widget.filteringStudyDescription(), "");
  CHECK_QSTRING(widget.filteringSeriesDescription(), "");
  CHECK_INT(widget.filteringDate(), ctkDICOMPatientItemWidget::DateType::Any);
  CHECK_INT(widget.numberOfOpenedStudiesPerPatient(), 2);
  CHECK_INT(widget.thumbnailSize(), ctkDICOMStudyItemWidget::ThumbnailSizeOption::Medium);

  // Test setting and getting
  widget.setPatientItem("1");
  CHECK_QSTRING(widget.patientItem(), "1");
  widget.setPatientID("123456");
  CHECK_QSTRING(widget.patientID(), "123456");
  widget.setFilteringStudyDescription("study");
  CHECK_QSTRING(widget.filteringStudyDescription(), "study");
  widget.setFilteringSeriesDescription("series");
  CHECK_QSTRING(widget.filteringSeriesDescription(), "series");
  widget.setFilteringDate(ctkDICOMPatientItemWidget::DateType::LastYear);
  CHECK_INT(widget.filteringDate(), ctkDICOMPatientItemWidget::DateType::LastYear);
  widget.setNumberOfOpenedStudiesPerPatient(6);
  CHECK_INT(widget.numberOfOpenedStudiesPerPatient(), 6);
  widget.setThumbnailSize(ctkDICOMStudyItemWidget::ThumbnailSizeOption::Small);
  CHECK_INT(widget.thumbnailSize(), ctkDICOMStudyItemWidget::ThumbnailSizeOption::Small);

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
