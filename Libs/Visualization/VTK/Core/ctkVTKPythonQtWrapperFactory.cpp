
/// CTK includes
#include "ctkVTKPythonQtWrapperFactory.h"

/// VTK includes
#include <vtkPythonUtil.h>
#include <vtkObject.h>
#include <vtkVersion.h>

//-----------------------------------------------------------------------------
ctkVTKPythonQtWrapperFactory::ctkVTKPythonQtWrapperFactory():Superclass()
{
}

//-----------------------------------------------------------------------------
ctkVTKPythonQtWrapperFactory::~ctkVTKPythonQtWrapperFactory()
{
}

//-----------------------------------------------------------------------------
PyObject* ctkVTKPythonQtWrapperFactory::wrap(const QByteArray& classname, void *ptr)
{
  if (classname.startsWith("vtk"))
    {
    return vtkPythonUtil::GetObjectFromPointer(reinterpret_cast<vtkObjectBase*>(ptr));
    }
  return NULL;
}

//-----------------------------------------------------------------------------
void* ctkVTKPythonQtWrapperFactory::unwrap(const QByteArray& classname, PyObject* object)
{
  if (classname.startsWith("vtk"))
    {
    return vtkPythonUtil::GetPointerFromObject(object, classname.data());
    }
  return NULL;
}
