#ifndef CTKDICOMHOSTMAINLOGIC_H
#define CTKDICOMHOSTMAINLOGIC_H

#include <QObject>
#include <QStringList>

#include "ctkDicomAppHostingTypes.h"

class ctkHostedAppPlaceholderWidget;
class ctkExampleDicomHost;
class ctkExampleHostControlWidget;
class ctkDICOMAppWidget;
class QModelIndex;
class QItemSelection;

class ctkDICOMHostMainLogic :
  public QObject
{
  Q_OBJECT
public:
  ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget*, ctkDICOMAppWidget*, QWidget* placeHolderForControls);
  virtual ~ctkDICOMHostMainLogic();
public slots:
  void configureHostedApp();
  void sendDataToHostedApp();
protected slots:
  void onTreeSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
  void publishSelectedData();
  void onAppReady();
  void placeHolderResized();
  void onDataAvailable();
  void aboutToQuit();
signals:
  void TreeSelectionChanged(const QString &);
  void SelectionValid(bool);
protected:
  ctkExampleDicomHost* Host;
  ctkExampleHostControlWidget* HostControls;
  ctkHostedAppPlaceholderWidget* PlaceHolderForHostedApp;
  ctkDICOMAppWidget* DicomAppWidget;
  QWidget* PlaceHolderForControls;
  ctkDicomAppHosting::AvailableData* Data;
  QString AppFileName;
  bool ValidSelection;
  QStringList SelectedFiles;
  bool SendData;
};

#endif
