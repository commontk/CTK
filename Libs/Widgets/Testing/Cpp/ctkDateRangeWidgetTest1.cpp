/*=========================================================================

  Library:   CTK

  Copyright (c) Isomics Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QSignalSpy>
#include <QTimer>

// CTK includes
#include "ctkDateRangeWidget.h"

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkDateRangeWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  QDateTime today = QDateTime(QDate::currentDate());
  QDateTime tomorrow = today.addDays(1);
  QDateTime yesterday = today.addDays(-1);
  QDateTime lastWeek = today.addDays(-7);
  QDateTime lastMonth = today.addMonths(-1);
  ctkDateRangeWidget dateRange;
  
  if (dateRange.displayTime())
    {
    std::cerr << "ctkDateRangeWidget::ctkDateRangeWidget(): "
              << "wrong default displayTime()" << std::endl;
    return EXIT_FAILURE;
    }
  if (!dateRange.isAnyDate())
    {
    std::cerr << "ctkDateRangeWidget::ctkDateRangeWidget(): "
              << "default date range failed: " 
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }
  
  dateRange.setToday();
  if (dateRange.isAnyDate() ||
      dateRange.startDateTime() != today ||
      dateRange.endDateTime() != tomorrow)
    {
    std::cerr << "ctkDateRangeWidget::setToday(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setYesterday();
  if (dateRange.isAnyDate() ||
      dateRange.startDateTime() != yesterday ||
      dateRange.endDateTime() != today)
    {
    std::cerr << "ctkDateRangeWidget::setYesterday(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setLastWeek();
  if (dateRange.isAnyDate() ||
      dateRange.startDateTime() != lastWeek ||
      dateRange.endDateTime() != today)
    {
    std::cerr << "ctkDateRangeWidget::setLastWeek(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setLastMonth();
  if (dateRange.isAnyDate() ||
      dateRange.startDateTime() != lastMonth ||
      dateRange.endDateTime() != today)
    {
    std::cerr << "ctkDateRangeWidget::setLastMonth(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setAnyDate();
  if (!dateRange.isAnyDate() ||
      dateRange.startDateTime() == lastMonth ||
      dateRange.endDateTime() == today)
    {
    std::cerr << "ctkDateRangeWidget::setAnyDate(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setDateTimeRange(QDateTime(QDate(-2, -9,12345678)),
                             QDateTime(QDate(2010, 15, 32)));
  if (!dateRange.isAnyDate() ||
      dateRange.startDateTime() == lastMonth ||
      dateRange.endDateTime() == today)
    {
    std::cerr << "ctkDateRangeWidget::setDateTimeRange(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  dateRange.setDateTimeRange(today, tomorrow);
  if (dateRange.isAnyDate() ||
      !dateRange.startDateTime().isValid() ||
      !dateRange.endDateTime().isValid())
    {
    std::cerr << "ctkDateRangeWidget::setDateTimeRange(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  QDateTime oldStart = dateRange.startDateTime();
  QDateTime oldEnd = dateRange.endDateTime();
  dateRange.setSelectRange();
  if (dateRange.startDateTime() != oldStart ||
      dateRange.endDateTime() != oldEnd)
    {
    std::cerr << "ctkDateRangeWidget::setSelectRange(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }
  
  dateRange.setDisplayTime(true);
  if (!dateRange.displayTime())
    {
    std::cerr << "ctkDateRangeWidget::setSelectRange(): "
              << qPrintable(dateRange.startDateTime().toString()) << " "
              << qPrintable(dateRange.endDateTime().toString()) << std::endl;
    return EXIT_FAILURE;
    }

  QSignalSpy startSpy(&dateRange, SIGNAL(startDateTimeChanged(QDateTime)));
  QSignalSpy endSpy(&dateRange, SIGNAL(endDateTimeChanged(QDateTime)));

  dateRange.setLastWeek();  
  if (startSpy.count() != 1 || endSpy.count() != 1)
    {
    std::cerr << "ctkDateRangeWidget::setLastWeek(): "
              << startSpy.count() << " " << endSpy.count() << std::endl;
    return EXIT_FAILURE;
    }
  startSpy.clear();
  endSpy.clear();

  dateRange.setLastMonth();  
  if (startSpy.count() != 1 || endSpy.count() != 0)
    {
    std::cerr << "ctkDateRangeWidget::setLastMonth(): "
              << startSpy.count() << " " << endSpy.count() << std::endl;
    return EXIT_FAILURE;
    }
  
  dateRange.show();
  if (argc < 2 || QString(argv[1]) != "-I" )
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }
  return app.exec();
}

