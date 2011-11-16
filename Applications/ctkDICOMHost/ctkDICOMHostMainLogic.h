#ifndef CTKDICOMHOSTMAINLOGIC_H
#define CTKDICOMHOSTMAINLOGIC_H

#include <QObject.h>
#include <QStringList.h>

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
  ctkDICOMHostMainLogic(ctkHostedAppPlaceholderWidget*, ctkDICOMAppWidget*);
  virtual ~ctkDICOMHostMainLogic();
public slots:
  void configureHostedApp();
  void sendDataToHostedApp();
protected slots:
  void onTreeSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
  void onStartProgress();
signals:
  void TreeSelectionChanged(const QString &);
protected:
  ctkExampleDicomHost* Host;
  ctkExampleHostControlWidget* HostControls;
  ctkHostedAppPlaceholderWidget* PlaceHolderForHostedApp;
  ctkDICOMAppWidget* DicomAppWidget;
  QString AppFileName;
  bool ValidSelection;
  QStringList SelectedFiles;
};

#endif
