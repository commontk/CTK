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

