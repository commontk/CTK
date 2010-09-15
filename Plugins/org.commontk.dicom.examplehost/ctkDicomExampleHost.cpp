#include "ctkDicomExampleHost.h"
#include <QProcess>
#include <QtDebug>

ctkDicomExampleHost::ctkDicomExampleHost() : ctkDicomAbstractHost(8080)
{
}

int ctkDicomExampleHost::StartApplication(QString AppPath, QString App_URL, int port){
    QStringList l;
    l.append("--hostURL");
    l.append("localhost:"+this->getPort());
    l.append("--applicationURL");
    l.append(App_URL+":"+QString(port));
    if (!QProcess::startDetached (
            AppPath,l))
    {
        qCritical() << "application failed to start!";
    }
}

void ctkDicomExampleHost::notifyStateChanged(ctkDicomWG23::State state){

}

void ctkDicomExampleHost::notifyStatus(const ctkDicomWG23::Status& status){

}
