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
#include "ctkDICOMSeriesItemWidget.h"

// Test visual browser import functionality
int ctkDICOMSeriesItemWidgetTest1(int argc, char* argv[])
{
  QApplication app(argc, argv);

  QStringList arguments = app.arguments();
  QString testName = arguments.takeFirst();
  Q_UNUSED(testName);

  bool interactive = arguments.removeOne("-I");

  ctkDICOMSeriesItemWidget widget;

  // Test the default values
  CHECK_QSTRING(widget.seriesItem(), "");
  CHECK_QSTRING(widget.patientID(), "");
  CHECK_QSTRING(widget.studyInstanceUID(), "");
  CHECK_QSTRING(widget.seriesInstanceUID(), "");
  CHECK_QSTRING(widget.seriesNumber(), "");
  CHECK_QSTRING(widget.modality(), "");
  CHECK_QSTRING(widget.seriesDescription(), "");
  CHECK_BOOL(widget.stopJobs(), false);
  CHECK_BOOL(widget.raiseJobsPriority(), false);
  CHECK_BOOL(widget.isCloud(), false);
  CHECK_BOOL(widget.isLoaded(), false);
  CHECK_BOOL(widget.isVisible(), false);
  CHECK_INT(widget.thumbnailSizePixel(), 200);

  // Test setting and getting
  widget.setSeriesItem("1");
  CHECK_QSTRING(widget.seriesItem(), "1");
  widget.setPatientID("123456");
  CHECK_QSTRING(widget.patientID(), "123456");
  widget.setStudyInstanceUID("123456.123");
  CHECK_QSTRING(widget.studyInstanceUID(), "123456.123");
  widget.setSeriesInstanceUID("123456.456");
  CHECK_QSTRING(widget.seriesInstanceUID(), "123456.456");
  widget.setSeriesNumber("1");
  CHECK_QSTRING(widget.seriesNumber(), "1");
  widget.setModality("CT");
  CHECK_QSTRING(widget.modality(), "CT");
  widget.setSeriesDescription("description");
  CHECK_QSTRING(widget.seriesDescription(), "description");
  widget.setStopJobs(true);
  CHECK_BOOL(widget.stopJobs(), true);
  widget.setRaiseJobsPriority(true);
  CHECK_BOOL(widget.raiseJobsPriority(), true);
  widget.setThumbnailSizePixel(100);
  CHECK_INT(widget.thumbnailSizePixel(), 100);

  if (!interactive)
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
