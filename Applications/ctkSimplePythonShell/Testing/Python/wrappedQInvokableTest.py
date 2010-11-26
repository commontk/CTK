import sys

if (_testWrappedQInvokableInstance.value() != 0):
  sys.exit(1)
  
_testWrappedQInvokableInstance.setValue(74)
if (_testWrappedQInvokableInstance.value() != 74):
  sys.exit(1)
  
sys.exit(0)
