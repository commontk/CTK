
// CTK includes
#include "ctkAbstractPythonManager.h"

// PythonQt includes
#include <PythonQt.h>

// STD includes
#include <cstdlib>
#include <iostream>

//-----------------------------------------------------------------------------
int ctkAbstractPythonManagerTest1(int argc, char * argv [] )
{
  Q_UNUSED(argc);
  Q_UNUSED(argv);

  ctkAbstractPythonManager pythonManager;

  if (pythonManager.isPythonInitialized())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isPythonInitialized()" << std::endl;
    return EXIT_FAILURE;
    }

  pythonManager.mainContext();

  if (!pythonManager.isPythonInitialized())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with isPythonInitialized()" << std::endl;
    return EXIT_FAILURE;
    }

  pythonManager.executeString("a = 6542");
  if (pythonManager.getVariable("a").toInt() != 6542)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with executeString()" << std::endl;
    return EXIT_FAILURE;
    }

  // Expected to return an empty value
  QVariant result = pythonManager.executeString("6542");
  if (!result.isNull())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with executeString()" << std::endl;
    return EXIT_FAILURE;
    }

  // Expected to fail
  result = pythonManager.executeString("b = 6542", ctkAbstractPythonManager::EvalInput);
  if (!result.isNull())
    {
    std::cerr << "Line " << __LINE__ << " - Problem with executeString()" << std::endl;
    return EXIT_FAILURE;
    }

  result = pythonManager.executeString("7", ctkAbstractPythonManager::EvalInput);
  if (result.toInt() != 7)
    {
    std::cerr << "Line " << __LINE__ << " - Problem with executeString()" << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
