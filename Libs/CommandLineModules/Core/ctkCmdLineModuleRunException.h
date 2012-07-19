/*===================================================================
  
BlueBerry Platform

Copyright (c) German Cancer Research Center, 
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without 
even the implied warranty of MERCHANTABILITY or FITNESS FOR 
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef CTKCMDLINEMODULERUNEXCEPTION_H
#define CTKCMDLINEMODULERUNEXCEPTION_H

#include <ctkException.h>

#include <QtCore>

class ctkCmdLineModuleRunException : public QtConcurrent::Exception, public ctkException
{
public:

  explicit ctkCmdLineModuleRunException(const QString& msg);

  ctkCmdLineModuleRunException(const QString& msg, const ctkCmdLineModuleRunException& cause);
  ctkCmdLineModuleRunException(const ctkCmdLineModuleRunException& o);

  ctkCmdLineModuleRunException& operator=(const ctkCmdLineModuleRunException& o);

  ~ctkCmdLineModuleRunException() throw();

  virtual const char* name() const throw();
  virtual const char* className() const throw();
  virtual ctkCmdLineModuleRunException* clone() const;
  virtual void rethrow() const;

  virtual void raise() const;

};

#endif // CTKCMDLINEMODULERUNEXCEPTION_H
