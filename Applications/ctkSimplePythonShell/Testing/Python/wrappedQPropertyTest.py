
import sys

if (_testWrappedQPropertyInstance.value != 0):
  sys.exit(1)
  
_testWrappedQPropertyInstance.value = 74
if (_testWrappedQPropertyInstance.value != 74):
  sys.exit(1)
  
sys.exit(0)
