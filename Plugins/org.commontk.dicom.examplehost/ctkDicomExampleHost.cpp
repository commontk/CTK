#include "ctkDicomExampleHost.h"
#include <QProcess>
#include <QtDebug>

ctkDicomExampleHost::ctkDicomExampleHost() : ctkDicomAbstractHost(8080)
{
}

void ctkDicomExampleHost::StartApplication(QString AppPath, const QUrl& App_URL){

    QStringList l;
    l.append("--hostURL");
    l.append(QString("localhost:") + this->getPort() );
    l.append("--applicationURL");
    l.append(App_URL.toString());
    if (!QProcess::startDetached (
            AppPath,l))
    {
        qCritical() << "application failed to start!";
    }

    this->appProcess.setProcessChannelMode(QProcess::ForwardedChannels);
    this->appProcess.start(AppPath,l);

}

QRect ctkDicomExampleHost::getAvailableScreen(const QRect& preferredScreen){
    qDebug()<< "set screen from preferredScreen:"<< preferredScreen;
    QRect rect (preferredScreen);
    emit giveAvailableScreen(rect);
    return rect;
}


void ctkDicomExampleHost::notifyStateChanged(ctkDicomWG23::State state){
    qDebug()<< "new state received:"<< static_cast<int>(state);
    emit stateChangedReceived(state);
}

void ctkDicomExampleHost::notifyStatus(const ctkDicomWG23::Status& status){
    qDebug()<< "new status received:"<<status.codeMeaning;
    emit statusReceived(status);;
}
