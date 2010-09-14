#ifndef CTKDICOMEXAMPLEHOST_H
#define CTKDICOMEXAMPLEHOST_H

#include <ctkDicomAbstractHost.h>
#include <org_commontk_dicom_examplehost_Export.h>

class org_commontk_dicom_examplehost_EXPORT ctkDicomExampleHost : public ctkDicomAbstractHost
{
public:
    ctkDicomExampleHost();

    virtual QString generateUID() { return ""; }
    virtual QRect getAvailableScreen(const QRect& preferredScreen) { return QRect(); }
    virtual QString getOutputLocation(const QStringList& preferredProtocols) { return ""; }
    virtual void notifyStateChanged(ctkDicomWG23::State state) {}
    virtual void notifyStatus(const ctkDicomWG23::Status& status) {}

};

#endif // CTKDICOMEXAMPLEHOST_H
