import qt, ctk

def app():
  return _ctkSimplePythonShellInstance

def quit():
  exit()

def exit():
  app().quit()
