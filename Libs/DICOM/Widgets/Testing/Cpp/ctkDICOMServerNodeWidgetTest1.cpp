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
#include <QTimer>
#include <QVariant>

// ctkDICOMCore includes
#include "ctkDICOMServerNodeWidget.h"

// STD includes
#include <iostream>

int ctkDICOMServerNodeWidgetTest1( int argc, char * argv [] )
{
  QApplication app(argc, argv);

  ctkDICOMServerNodeWidget widget;
  if (widget.callingAETitle() != "FINDSCU")
    {
    std::cout << "ctkDICOMServerNodeWidget::callingAETitle() failed:"
              << qPrintable(widget.callingAETitle()) << " instead of FINDSCU"
              << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.storageAETitle() != "CTKSTORE")
    {
    std::cout << "ctkDICOMServerNodeWidget::storageAETitle() failed:"
              << qPrintable(widget.storageAETitle()) << " instead of CTKSTORE"
              << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.storagePort() != 11112)
    {
    std::cout << "ctkDICOMServerNodeWidget::storagePort() failed:"
              << widget.storagePort() << " instead of 11112"
              << std::endl;
    return EXIT_FAILURE;
    }
  QMap<QString,QVariant> parameters;
  parameters["CallingAETitle"] = QVariant(widget.callingAETitle());
  parameters["StorageAETitle"] = QVariant(widget.storageAETitle());
  parameters["StoragePort"] = QVariant(widget.storagePort());
  if (widget.parameters() != parameters)
    {
    std::cout << "ctkDICOMServerNodeWidget::parameters() failed." << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.serverNodes().count() != 1)
    {
    std::cout << "ctkDICOMServerNodeWidget::serverNodes() failed:"
              << widget.serverNodes().count() << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.selectedServerNodes().count() != 1)
    {
    std::cout << "ctkDICOMServerNodeWidget::selectedServerNodes() failed:"
              << widget.selectedServerNodes().count() << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.serverNodeParameters("").count() != 0)
    {
    std::cout << "ctkDICOMServerNodeWidget::serverNodeParameters("") failed:"
              << widget.serverNodeParameters("").count() << std::endl;
    return EXIT_FAILURE;
    }
  if (widget.serverNodeParameters(-1).count() != 0 ||
      widget.serverNodeParameters(0).count() != 5 || // 5 per node
      widget.serverNodeParameters(1).count() != 0)
    {
    std::cout << "ctkDICOMServerNodeWidget::serverNodeParameters() failed:"
              << widget.serverNodeParameters(-1).count() << " "
              << widget.serverNodeParameters(0).count() << " "
              << widget.serverNodeParameters(1).count() << std::endl;
    return EXIT_FAILURE;
    }
  // Add an empty row and make it current
  widget.addServerNode();
  if (widget.serverNodes().count() != 2)
    {
    std::cout << "ctkDICOMServerNodeWidget::addServerNode() failed:"
              << widget.serverNodes().count() << std::endl;
    return EXIT_FAILURE;
    }
  widget.removeCurrentServerNode();
  if (widget.serverNodes().count() != 1)
    {
    std::cout << "ctkDICOMServerNodeWidget::removeCurrentServerNode() failed:"
              << widget.serverNodes().count() << std::endl;
    return EXIT_FAILURE;
    }
  QMap<QString, QVariant> serverNode;
  serverNode["Name"] = QString("TestName");
  serverNode["CheckState"] = Qt::Unchecked;
  serverNode["AETitle"] = QString("TestAETitle");
  serverNode["Address"] = QString("TestAddress");
  serverNode["Port"] = 12345;
  widget.addServerNode(serverNode);
  if (widget.serverNodes().count() != 2 ||
      widget.serverNodes()[1] != "TestName" ||
      widget.selectedServerNodes().count() != 1 ||
      widget.serverNodeParameters("TestName") != serverNode ||
      widget.serverNodeParameters(1) != serverNode)
    {
     std::cout << "ctkDICOMServerNodeWidget::addServerNode() failed:"
              << widget.serverNodes().count() << " "
              << (widget.serverNodes().count() > 1?
                    qPrintable(widget.serverNodes()[1]) : "none") << " "
              << widget.selectedServerNodes().count() << " "
              << (widget.serverNodeParameters("TestName") == serverNode) << " "
              << (widget.serverNodeParameters(1) == serverNode) << std::endl;
    return EXIT_FAILURE;
    }
  widget.removeCurrentServerNode();
  if (widget.serverNodes().count() != 1)
    {
    std::cout << "ctkDICOMServerNodeWidget::removeCurrentServerNode() failed:"
              << widget.serverNodes().count() << std::endl;
    return EXIT_FAILURE;
    }
  widget.saveSettings();
  widget.readSettings();
  widget.show();

  if (argc <= 1 || QString(argv[1]) != "-I")
    {
    QTimer::singleShot(200, &app, SLOT(quit()));
    }

  return app.exec();
}
