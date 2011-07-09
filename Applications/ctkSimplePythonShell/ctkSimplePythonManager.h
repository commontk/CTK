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

#ifndef __ctkSimplePythonManager_h
#define __ctkSimplePythonManager_h

// CTK includes
# include <ctkAbstractPythonManager.h>

class PythonQtObjectPtr;

class ctkSimplePythonManager : public ctkAbstractPythonManager
{
  Q_OBJECT

public:
  typedef ctkAbstractPythonManager Superclass;
  ctkSimplePythonManager(QObject* parent=0);
  ~ctkSimplePythonManager();
  
protected:

  virtual QStringList pythonPaths();
  virtual void preInitialization();
  virtual void executeInitializationScripts();

};


#endif

