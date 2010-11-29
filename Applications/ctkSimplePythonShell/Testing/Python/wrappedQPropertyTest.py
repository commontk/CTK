
import sys

if (_testWrappedQPropertyInstance.Value != 0):
  sys.exit(1)
  
_testWrappedQPropertyInstance.Value = 74
if (_testWrappedQPropertyInstance.Value != 74):
  sys.exit(1)
  
sys.exit(0)
