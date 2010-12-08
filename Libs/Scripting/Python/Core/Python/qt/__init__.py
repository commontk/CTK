""" This module loads all the classes from the wrapped Qt libraries into
its namespace."""

__kits_to_load = [
'Core',
'Gui', 
'Network', 
'OpenGL', 
'Sql',
'Svg',
'UiTools',
'WebKit',
'Xml', 
'XmlPatterns'
]

# Set to True when debugging
CTK_VERBOSE_IMPORT = False

for kit in __kits_to_load:
   try:
     exec "from PythonQt.Qt%s import *" % (kit)
   except ImportError as detail:
     if CTK_VERBOSE_IMPORT:
       print detail
   
# Removing things the user shouldn't have to see.
del __kits_to_load
