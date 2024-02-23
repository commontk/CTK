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

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QList>
#include <QPixmap>
#include <QTimer>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkThumbnailListWidget.h"

// STD includes
#include <iostream>

int ctkThumbnailListWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkThumbnailListWidget widget;

  QPixmap pix1(QSize(128, 128));;
  QPixmap pix2(QSize(64, 64));
  QPixmap pix3(QSize(256, 256));
  QPixmap pix4(QSize(256, 128));


  pix1.fill(Qt::green);
  pix2.fill(Qt::yellow);
  pix3.fill(Qt::blue);
  pix4.fill(Qt::red);

  QList<QPixmap> pixList;

  widget.setThumbnailSize(QSize(128, 128));
  if(widget.thumbnailSize() != QSize(128, 128))
  {
      std::cerr << "ctkThumbnailListWidget::setThumbnailWidth failed."
        << " size: " << widget.thumbnailSize().width()
        << "," << widget.thumbnailSize().height()
        << " expected: 128" << std::endl;
      return EXIT_FAILURE;
  }

  pixList.append(pix1);
  pixList.append(pix2);
  pixList.append(pix3);
  pixList.append(pix4);

  widget.addThumbnails(pixList);

  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(200, &app, SLOT(quit()));
  }

  return app.exec();
}
