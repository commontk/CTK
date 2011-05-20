/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
 All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer

=========================================================================auto=*/

#ifndef __ctkDICOMCorePythonQtDecorators_h
#define __ctkDICOMCorePythonQtDecorators_h

// Qt includes
#include <QObject>

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkWorkflowStep.h>
#include <ctkWorkflowTransitions.h>

#include "ctkDICOMCoreExport.h"

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

/// \ingroup DICOMCore
class CTK_DICOM_CORE_EXPORT ctkDICOMCorePythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkDICOMCorePythonQtDecorators()
    {
    }

public slots:

  //
  // None yet - refer to other libs for examples
  //

};

#endif
