import sys

if (_testWrappedSlotInstance.value() != 0):
  sys.exit(1)
  
_testWrappedSlotInstance.setValue(74)
if (_testWrappedSlotInstance.value() != 74):
  sys.exit(1)
  
sys.exit(0)
