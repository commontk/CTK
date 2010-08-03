
// CTK includes
#include "ctkSimplePythonManager.h"
#include "ctkSimplePythonQtDecorators.h"

//-----------------------------------------------------------------------------
ctkSimplePythonManager::ctkSimplePythonManager(QObject* _parent) : Superclass(_parent)
{

}

//-----------------------------------------------------------------------------
ctkSimplePythonManager::~ctkSimplePythonManager()
{
}

//-----------------------------------------------------------------------------
QStringList ctkSimplePythonManager::pythonPaths()
{  
  QStringList paths;  
  
  return paths; 
}

//-----------------------------------------------------------------------------
void ctkSimplePythonManager::preInitialization()
{
  Superclass::preInitialization();

  // Register decorators
  this->registerPythonQtDecorator(new ctkSimplePythonQtDecorators(this));  
  
  // Add object to python interpreter context
  //this->addObjectToPythonMain("_qSlicerCoreApplicationInstance", app);

  // Evaluate application script
  //this->executeFile(app->slicerHome() + "/bin/Python/slicer/slicerqt.py");
}
