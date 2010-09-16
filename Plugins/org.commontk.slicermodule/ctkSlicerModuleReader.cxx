/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/SlicerExecutionModel/ctkSlicerModuleReader/ctkSlicerModuleReader.cxx $
  Date:      $Date: 2010-07-22 21:09:03 +0200 (jue, 22 jul 2010) $
  Version:   $Revision: 14306 $

==========================================================================*/
#include "ctkSlicerModuleReader.h"
#include <QtXml/QDomDocument>


void ctkSlicerModuleReader::Update()
{
  QDomDocument domDocument;
  QString errorStr;
  int errorLine;
  int errorColumn;

  if (!domDocument.setContent(xmlContent(), &errorStr, &errorLine,
    &errorColumn)) {
      throw std::runtime_error( 
        tr("Parse error at line %1, column %2:\n%3")
        .arg(errorLine)
        .arg(errorColumn)
        .arg(errorStr).toStdString().c_str() );
      return ;
  }

}
