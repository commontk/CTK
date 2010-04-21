/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __ctkXIPAdaptor_h
#define __ctkXIPAdaptor_h

// Qt includes 
#include <QObject>

// CTK includes
#include <ctkPimpl.h>

#include "CTKVisualizationXIPExport.h"

class ctkXIPAdaptorPrivate;
class CTK_VISUALIZATION_XIP_EXPORT ctkXIPAdaptor : public QObject
{
  Q_OBJECT
public:
  typedef QObject Superclass;
  explicit ctkXIPAdaptor(QObject* parent = 0);
  virtual ~ctkXIPAdaptor();
  
private:
  CTK_DECLARE_PRIVATE(ctkXIPAdaptor);
};

#endif
