#ifndef CTKDICOMEXAMPLEHOST_H
#define CTKDICOMEXAMPLEHOST_H

#include <ctkDicomAbstractHost.h>
#include <org_commontk_dicom_examplehost_Export.h>

#include <QUrl>

class org_commontk_dicom_examplehost_EXPORT ctkDicomExampleHost : public ctkDicomAbstractHost{
    Q_OBJECT
public:
    ctkDicomExampleHost();

    virtual void StartApplication(QString AppPath, const QUrl& App_URL = QUrl("http://localhost:8081/"));
    virtual QString generateUID() { return ""; }
    virtual QRect getAvailableScreen(const QRect& preferredScreen);
    virtual QString getOutputLocation(const QStringList& /*preferredProtocols*/) { return ""; }
    virtual void notifyStateChanged(ctkDicomWG23::State state);
    virtual void notifyStatus(const ctkDicomWG23::Status& status);

signals:
    void stateChangedReceived(ctkDicomWG23::State state);
    void statusReceived(const ctkDicomWG23::Status& status);
    void giveAvailableScreen(QRect rect);
};

#endif // CTKDICOMEXAMPLEHOST_H
