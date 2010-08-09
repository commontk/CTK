
// CTK includes
#include "ctkSimplePythonManager.h"
#include "ctkSimplePythonQtDecorators.h"

// PythonQT includes
#include <PythonQt.h>

// PythonQt wrapper initialization methods
void PythonQt_init_org_commontk_CTKCore(PyObject*);
void PythonQt_init_org_commontk_CTKWidgets(PyObject*);

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

  // Initialize wrappers
  PythonQt_init_org_commontk_CTKCore(0);
  PythonQt_init_org_commontk_CTKWidgets(0);

  // Register decorators
  this->registerPythonQtDecorator(new ctkSimplePythonQtDecorators(this));  
  
  // Add object to python interpreter context
  //this->addObjectToPythonMain("_qSlicerCoreApplicationInstance", app);

  // Evaluate application script
  //this->executeFile(app->slicerHome() + "/bin/Python/slicer/slicerqt.py");
}
