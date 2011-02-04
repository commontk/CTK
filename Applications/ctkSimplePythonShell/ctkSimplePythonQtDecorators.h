
#ifndef __ctkSimplePythonQtDecorators_h
#define __ctkSimplePythonQtDecorators_h

// CTK includes
#include "ctkSimplePythonShellConfigure.h" // For CTK_WRAP_PYTHONQT_LIGHT
#include <ctkAbstractPythonManager.h>
#ifdef CTK_WRAP_PYTHONQT_LIGHT
# include <ctkCorePythonQtDecorators.h>
# include <ctkWidgetsPythonQtDecorators.h>
#endif

// PythonQt includes
#include <PythonQt.h>

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
#ifdef CTK_WRAP_PYTHONQT_LIGHT
    pythonManager->registerPythonQtDecorator(new ctkCorePythonQtDecorators);
    pythonManager->registerPythonQtDecorator(new ctkWidgetsPythonQtDecorators);
#else
    Q_UNUSED(pythonManager);
#endif
    }

public slots:

  
  
};

#endif

