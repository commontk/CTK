#ifndef CTKEXAMPLEDICOMHOST_H
#define CTKEXAMPLEDICOMHOST_H

#include <ctkDicomAbstractHost.h>
#include <ctkHostedAppPlaceholderWidget.h>

#include <org_commontk_dah_examplehost_Export.h>

#include <QUrl>
#include <QProcess>

class org_commontk_dah_examplehost_EXPORT ctkExampleDicomHost : public ctkDicomAbstractHost{
    Q_OBJECT
public:
    ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort = 8080, int appPort = 8081);

    virtual void StartApplication(QString AppPath);
    virtual QString generateUID() { return ""; }
    virtual QRect getAvailableScreen(const QRect& preferredScreen);
    virtual QString getOutputLocation(const QStringList& /*preferredProtocols*/) { return ""; }
    virtual void notifyStateChanged(ctkDicomAppHosting::State state);
    virtual void notifyStatus(const ctkDicomAppHosting::Status& status);
    // exchange methods
    virtual bool notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData);
    virtual QList<ctkDicomAppHosting::ObjectLocator>* getData(
      QList<QUuid> objectUUIDs, 
      QList<QString> acceptableTransferSyntaxUIDs, 
      bool includeBulkData);
    virtual void releaseData(QList<QUuid> objectUUIDs);
    
    const QProcess& getAppProcess() const { return appProcess; }
    ~ctkExampleDicomHost();

signals:
    void stateChangedReceived(ctkDicomAppHosting::State state);
    void statusReceived(const ctkDicomAppHosting::Status& status);
    void giveAvailableScreen(QRect rect);

protected:
    QProcess appProcess;
    ctkHostedAppPlaceholderWidget* placeholderWidget;
    ctkDicomAppHosting::State applicationState;
  protected slots:
    void forwardConsoleOutput();
};

#endif // CTKEXAMPLEDICOMHOST_H
