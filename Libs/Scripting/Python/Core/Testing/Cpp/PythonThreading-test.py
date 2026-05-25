import threading
from PythonQt.QtCore import QCoreApplication, QThread


main_thread = QThread.currentThread()

_state = {}
_done = threading.Event()


class ThreadCheck(QThread):
    def run(self):
        _state['is_main_thread'] = (QThread.currentThread() == main_thread)
        _state['is_application_thread'] = (
                QThread.currentThread() == QCoreApplication.instance().thread())
        _state['python_ran'] = (sum(range(5)) == 10)
        QThread.msleep(10)
        _done.set()


worker = ThreadCheck()
worker.start()

time_out = not _done.wait(10)
if time_out:
    _state['wait_ok'] = False
else:
    _state['wait_ok'] = worker.wait(5000)

test_is_main_thread = _state.get('is_main_thread')
test_is_application_thread = _state.get('is_application_thread')
test_python_ran = _state.get('python_ran')
test_wait_ok = _state.get('wait_ok')
