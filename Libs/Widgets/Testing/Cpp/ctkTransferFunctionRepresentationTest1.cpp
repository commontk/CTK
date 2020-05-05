/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QCoreApplication>
#include <QColor>
#include <QVariant>
#include <QLocale>
#include <QRectF>
#include <QPainterPath>
#include <QGradient>
#include <QList>

// CTK includes
#include "ctkTransferFunctionRepresentation.h"
#include "ctkTransferFunction.h"

// STL includes
#include <cstdlib>
#include <iostream>
#include <limits>

int ctkTransferFunctionRepresentationTest1(int argc, char * argv [])
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

//--------------------------------------------------------------
// Test 1 : test without transfert function
//--------------------------------------------------------------

  //---------Test Constructor----------  
  ctkTransferFunctionRepresentation representation;
  if(representation.transferFunction() != 0)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::"
              << " ctkTransfertFunctionRepresentation - transfertFunction not null." 
              << std::endl;
    return EXIT_FAILURE;
    }

  QColor expectedVerticalGradientColor = QColor::fromRgbF(1., 0., 0., 1.);
  if(representation.verticalGradientColor() != expectedVerticalGradientColor)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation:: "
              << "ctkTransfertFunctionRepresentation"
              << std::endl;
    return EXIT_FAILURE;
    }

  //---------Test setVerticalGradientColor------
  expectedVerticalGradientColor = QColor::fromRgbF(0., 1., 1., 0.);
  representation.setVerticalGradientColor(expectedVerticalGradientColor);
  if(representation.verticalGradientColor() != expectedVerticalGradientColor)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::setVerticalGradientColor"
              << std::endl;
    return EXIT_FAILURE;
    }

  //---------Test PosX--------------------------
  qreal expectedPosX = 2.;
  if (representation.posX(expectedPosX) != expectedPosX)
  {
  std::cerr << "Line " << __LINE__ 
            << " - Problem with ctkTransfertFunctionRepresentation::posX" 
            << std::endl;
  return EXIT_FAILURE;
  }

  ctkControlPoint cp;
  if (representation.posX(&cp) != cp.x())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::posX"
              << representation.posX(&cp);
    return EXIT_FAILURE;
    }

  ctkPoint point;
  if (representation.posX(point) != point.X)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::posX"
              << representation.posX(point);
    return EXIT_FAILURE;
    }

  //---------Test PosY--------------------------
  QVariant variant = 2.;
  if (representation.posY(variant) != 2.)  
    {
    std::cerr << "Line " << __LINE__ 
              << " - Problem with ctkTransfertFunctionRepresentation::posY"
              << " - for the qreal" << std::endl;
    return EXIT_FAILURE;
    }

  QColor defaultColor = QColor::fromRgbF(0., 0., 0., 0.);
  variant = defaultColor;
  if (representation.posY(variant) != defaultColor.alphaF())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::posY"
              << " - for the color: " << representation.posY(variant);
    return EXIT_FAILURE;
    } 

  if (representation.posY(&cp) != cp.value())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::posY"
              << representation.posY(&cp);
    return EXIT_FAILURE;
    }
  if (representation.posY(point) != point.Value)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::posY"
              << representation.posX(point);
    return EXIT_FAILURE;
    }
 
  //--------Test Color--------------------------
  QColor expectedColor = QColor::fromRgbF(1., 1., 1.);
  variant = expectedColor;
  if (representation.color(variant) != expectedColor)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::color"
              << " when QVariant is a color"
              << std::endl;
    return EXIT_FAILURE;
    }
  variant = expectedPosX;
  if (representation.color(variant) != QColor::fromRgbF(0., 0., 0.))
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::color"
              << " when QVariant is a not a color"
              << std::endl;
    return EXIT_FAILURE;
    }
  if (representation.color(&cp) != representation.color(cp.value()))
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::color";
    return EXIT_FAILURE;
    }
  if (representation.color(point) != representation.color(point.Value))
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with ctkTransfertFunctionRepresentation::color";
    return EXIT_FAILURE;
    }

  //---------Test MapXToScene------------------
  qreal xPos = 2.;
  if (representation.mapXToScene(xPos) != 0) 
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapXToScene   "
              << representation.mapXToScene(xPos)
              << std::endl;
    return EXIT_FAILURE;
    }

  //--------Test MapYToScene------------------
  qreal yPos = 2.;
  if (representation.mapYToScene(yPos) != 1) //Because la fonction height return 1.
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapYToScene   "
              << representation.mapYToScene(yPos)
              << std::endl;
    return EXIT_FAILURE;
    } 

  //--------Test MapXFromScene----------------
  qreal defaultScenePosX = 2.;
  qreal mapX = representation.mapXFromScene(defaultScenePosX);
  if (mapX != std::numeric_limits<qreal>::infinity())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapXFromScene  "
              << representation.mapXFromScene(defaultScenePosX)
              << std::endl;
    return EXIT_FAILURE;
    } 

  //--------Test MapyFromScene----------------  
  qreal defaultScenePosY = 2.;
  qreal mapY = representation.mapYFromScene(defaultScenePosY);
  if (mapY != - std::numeric_limits<qreal>::infinity())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapyFromScene  "
              << representation.mapYFromScene(defaultScenePosY)
              << std::endl;
    return EXIT_FAILURE;
    } 

  //--------Test Curve-----------------------
  QPainterPath defaultPath = representation.curve();
  if(!defaultPath.isEmpty())
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::curve  "
              << std::endl;
    return EXIT_FAILURE;
    }

  //--------Test gradient--------------------
  if(representation.gradient().type() != QGradient::LinearGradient)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::gradient  "
              << std::endl;
    return EXIT_FAILURE;   
    }
  
  //--------Test points---------------------- 
  QList<QPointF> expectedPoints;
  if(representation.points().size() != 0)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::points  "
              << std::endl;
    return EXIT_FAILURE; 
    }
  
  //--------Test bezierParams----------------
  ctkControlPoint startPoint;
  ctkControlPoint endPoint;
  if(representation.bezierParams(&startPoint,&endPoint).size() != 4)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::bezierParams "
              << std::endl;
    return EXIT_FAILURE; 
    }

  //--------Test nonLinearPoints------------- // ? case subpoint ?
  if(representation.nonLinearPoints(&startPoint,&endPoint).size() != 2)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::nonLinearPoints "
              << std::endl;
    return EXIT_FAILURE; 
    }
  
  //-------Test mapPointToScene-With q ctkPoint*----------
  qreal defaultX = 2.;
  QVariant defaultVariant = 1.;  
  ctkPoint defaultPoint(defaultX,defaultVariant);  
  QPointF defaultPointF(0,1);
  if(representation.mapPointToScene(defaultPoint) != defaultPointF)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapPointFromScene  "
              << defaultPointF.x()
              << "   "
              << defaultPointF.y()
              << std::endl;
    return EXIT_FAILURE; 
    }
  
  //-------Test mapPointToScene-With a ctkControlPoint&------------  
  ctkControlPoint defaultControlPoint;     
  if(representation.mapPointToScene(&defaultControlPoint) != defaultPointF)
    {
    std::cerr << "Line " << __LINE__
              << " - Problem with "
              << "ctkTransfertFunctionRepresentation::mapPointFromScene  "
              << defaultPointF.x()
              << "   "
              << defaultPointF.y()
              << std::endl;
    return EXIT_FAILURE; 
    }
  
  //-------Test computeCurve----------------
  representation.computeCurve();

  //-------Test computegradient-------------
  representation.computeGradient();

  return EXIT_SUCCESS;
}

