
#ifndef __ctkSimplePythonQtDecorators_h
#define __ctkSimplePythonQtDecorators_h

// CTK includes
#include <ctkAbstractPythonManager.h>

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

class ctkSimplePythonQtDecorators : public QObject
{
  Q_OBJECT

public:

  ctkSimplePythonQtDecorators(ctkAbstractPythonManager* pythonManager)
    {
    Q_ASSERT(pythonManager);
    //pythonManager->registerClassForPythonQt(&qSlicerCoreApplication::staticMetaObject);
    //pythonManager->registerClassForPythonQt(&qSlicerModuleManager::staticMetaObject);
    //pythonManager->registerClassForPythonQt(&qSlicerAbstractModule::staticMetaObject);
    //pythonManager->registerClassForPythonQt(&qSlicerAbstractModuleWidget::staticMetaObject);
    //pythonManager->registerCPPClassForPythonQt("qSlicerModuleFactoryManager");
    }

public slots:

  
  
};

#endif

