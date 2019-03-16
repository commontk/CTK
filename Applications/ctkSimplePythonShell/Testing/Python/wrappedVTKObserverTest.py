from __future__ import print_function

import qt

# Importing vtk initializes vtkPythonMap owned by vtkPythonUtil and prevent
# call to vtkPythonUtil::GetObjectFromPointer() from segfaulting.
# PythonQt internally uses vtkPythonUtil to properly wrap/unwrap VTK objects
from vtk import *

t = _testWrappedVTKObserverInstance.getTable()

def onTableModified(caller, event):
    print("Table modified !")

t.AddObserver(vtkCommand.ModifiedEvent, onTableModified)
t.Modified()

qt.QApplication.exit(0)
