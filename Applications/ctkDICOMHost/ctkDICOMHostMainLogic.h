#ifndef CTKDICOMHOSTMAINLOGIC_H
#define CTKDICOMHOSTMAINLOGIC_H

#include <QObject.h>

class ctkHostedAppPlaceholderWidget;
class ctkExampleDicomHost;
class ctkExampleHostControlWidget;

class ctkDICOMHostMainLogic :
  public QObject
{
  Q_OBJECT
public:
  ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget*);
  virtual ~ctkDICOMHostMainLogic();
public slots:
  void configureHostedApp();
protected:
  ctkExampleDicomHost* Host;
  ctkExampleHostControlWidget* HostControls;
  ctkHostedAppPlaceholderWidget* placeHolderForHostedApp;
  QString AppFileName;
};

#endif
