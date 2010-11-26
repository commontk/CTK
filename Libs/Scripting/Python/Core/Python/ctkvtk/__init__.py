""" This module loads a subset of the VTK classes into its namespace."""

import os

__kits_to_load = [
"Common",
"Filtering",
"Rendering",
"Graphics",
"Hybrid",
"Views",
"Infovis",
"Widgets",
"Imaging",
"IO"]

prefix = 'vtk'
if os.name == 'posix': prefix = "libvtk"

for kit in __kits_to_load:
  try:
    exec "from %s%sPython import *" % (prefix, kit)
  except ImportError as detail:
     print detail
  
from vtk.util.vtkVariant import *
from vtk import vtkImageScalarTypeNameMacro

# Removing things the user shouldn't have to see.
del __kits_to_load, os
