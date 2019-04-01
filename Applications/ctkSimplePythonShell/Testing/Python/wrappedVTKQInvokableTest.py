from __future__ import print_function

import qt

# Importing vtk initializes vtkPythonMap owned by vtkPythonUtil and prevent
# call to vtkPythonUtil::GetObjectFromPointer() from segfaulting.
# PythonQt internally uses vtkPythonUtil to properly wrap/unwrap VTK objects
from vtk import *

t = _testWrappedVTKQInvokableInstance.getTable()
print(t.GetClassName())

t2 = vtkTable()
_testWrappedVTKQInvokableInstance.setTable(t2)
if _testWrappedVTKQInvokableInstance.getTable() != t2:
  qt.QApplication.exit(1)

qt.QApplication.exit(0)
