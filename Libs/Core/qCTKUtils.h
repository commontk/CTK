/*=========================================================================

  Library:   qCTK

  Copyright (c) Kitware Inc. 
  All rights reserved.
  Distributed under a BSD License. See LICENSE.txt file.

  This software is distributed "AS IS" WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the above copyright notice for more information.

=========================================================================*/

#ifndef __qCTKUtils_h
#define __qCTKUtils_h

// QT includes
#include <QStringList>

// STD includes
#include <vector>

#include "CTKCoreExport.h"

class Q_CTK_CORE_EXPORT qCTKUtils
{
  
public:
  typedef qCTKUtils Self;

  ///
  /// Convert a QStringList to Vector of char*
  /// Caller will be responsible to delete the content of the vector
  static void qListToSTLVector(const QStringList& list, std::vector<char*>& vector);

  ///
  /// Convert a QStringList to a Vector of string
  static void qListToSTLVector(const QStringList& list, std::vector<std::string>& vector);

  ///
  /// Convert a Vector of string to QStringList
  static void stlVectorToQList(const std::vector<std::string>& vector, QStringList& list);

private:
  /// Not implemented
  qCTKUtils(){}
  virtual ~qCTKUtils(){}

};

#endif
