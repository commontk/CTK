from __future__ import print_function

#
# Copied from VTK/Common/Testing/Python/PythonSmoke.py
#

import qt

try:
  import vtk

except:
  print("Cannot import vtk")
  qt.QApplication.exit(1)
try:
  print(dir(vtk))
except:
  print("Cannot print dir(vtk)")
  qt.QApplication.exit(1)

try:
  try:
    try:
      o = vtk.vtkLineWidget()
      print("Using Hybrid")
    except:
      o = vtk.vtkActor()
      print("Using Rendering")
  except:
    o = vtk.vtkObject()
    print("Using Common")
except:
  print("Cannot create vtkObject")
  qt.QApplication.exit(1)

try:
  print(o)
  print("Reference count: %d" % o.GetReferenceCount())
  print("Class name: %s" % o.GetClassName())
except:
  print("Cannot print object")
  qt.QApplication.exit(1)

try:
  b = vtk.vtkObject()
  d = b.SafeDownCast(o)
  print((b, d))
except:
  print("Cannot downcast")
  qt.QApplication.exit(1)

qt.QApplication.exit(0)

