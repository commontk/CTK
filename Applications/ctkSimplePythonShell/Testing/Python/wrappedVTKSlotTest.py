from __future__ import print_function

import qt

# Importing vtk initializes vtkPythonMap owned by vtkPythonUtil and prevent
# call to vtkPythonUtil::GetObjectFromPointer() from segfaulting.
# PythonQt internally uses vtkPythonUtil to properly wrap/unwrap VTK objects
from vtk import *

t = _testWrappedVTKSlotInstance.getTable()
print(t.GetClassName())

t2 = vtkTable()
_testWrappedVTKSlotInstance.setTable(t2)
if _testWrappedVTKSlotInstance.getTable() != t2:
  qt.QApplication.exit(1)

qt.QApplication.exit(0)
