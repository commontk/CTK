#ifndef CTKDICOMHOSTMAINLOGIC_H
#define CTKDICOMHOSTMAINLOGIC_H

#include <QObject>
#include <QStringList>

#include "ctkDicomAppHostingTypes.h"

#include <org_commontk_dah_examplehost_Export.h>

class ctkHostedAppPlaceholderWidget;
class ctkExampleDicomHost;
class ctkExampleHostControlWidget;
class ctkDICOMAppWidget;
class QModelIndex;
class QItemSelection;

class ctkExampleDicomHost;

class org_commontk_dah_examplehost_EXPORT ctkExampleHostLogic :
  public QObject
{
  Q_OBJECT
public:
  ctkExampleHostLogic(ctkHostedAppPlaceholderWidget*, QWidget* placeHolderForControls, int hostPort = 8080, int appPort = 8081);
  virtual ~ctkExampleHostLogic();
  ctkExampleDicomHost* getHost();
  ctkExampleHostControlWidget* getHostControls();
public slots:
  void configureHostedApp();
  void sendData(ctkDicomAppHosting::AvailableData& data, bool lastData);
protected slots:
  void publishSelectedData();
  void onAppReady();
  void placeHolderResized();
  void aboutToQuit();
protected:
  ctkExampleDicomHost* Host;
  ctkExampleHostControlWidget* HostControls;
  ctkHostedAppPlaceholderWidget* PlaceHolderForHostedApp;
  QWidget* PlaceHolderForControls;
  ctkDicomAppHosting::AvailableData* Data;
  QString AppFileName;
  //bool ValidSelection;
  bool LastData;
  bool SendData;
};

#endif
