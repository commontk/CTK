
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
#if (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION <= 6) || VTK_MAJOR_VERSION < 5
    return vtkPythonGetObjectFromPointer(reinterpret_cast<vtkObjectBase*>(ptr));
#else
    return vtkPythonUtil::GetObjectFromPointer(reinterpret_cast<vtkObjectBase*>(ptr));
#endif
    }
  return NULL;
}

//-----------------------------------------------------------------------------
void* ctkVTKPythonQtWrapperFactory::unwrap(const QByteArray& classname, PyObject* object)
{
  if (classname.startsWith("vtk"))
    {
#if (VTK_MAJOR_VERSION == 5 && VTK_MINOR_VERSION <= 6) || VTK_MAJOR_VERSION < 5
    return vtkPythonGetPointerFromObject(object, classname.data());
#else
    return vtkPythonUtil::GetPointerFromObject(object, classname.data());
#endif
    }
  return NULL;
}
