#include "ctkDicomExampleHost.h"
#include "ctkDicomWG23TypesHelper.h"

#include <QProcess>
#include <QtDebug>

ctkDicomExampleHost::ctkDicomExampleHost(QWidget* placeholderWidget, int hostPort, int appPort) :
    ctkDicomAbstractHost(hostPort, appPort),
    placeholderWidget(placeholderWidget),
    applicationState(ctkDicomWG23::IDLE)
{
}

void ctkDicomExampleHost::StartApplication(QString AppPath){

    QStringList l;
    l.append("--hostURL");
    l.append(QString("http://localhost:") + QString::number(this->getHostPort()) );
    l.append("--applicationURL");
    l.append(QString("http://localhost:") + QString::number(this->getAppPort()));
    l.append("dicomapp"); // the app plugin to use - has to be changed later
    if (!QProcess::startDetached (
            AppPath,l))
    {
        qCritical() << "application failed to start!";
    }
    qDebug() << "starting application: " << AppPath << " " << l;

    this->appProcess.start(AppPath,l);

}

QRect ctkDicomExampleHost::getAvailableScreen(const QRect& preferredScreen){
    qDebug()<< "set screen from preferredScreen:"<< preferredScreen;
    QRect rect (preferredScreen);
    emit giveAvailableScreen(rect);
    return rect;
}


void ctkDicomExampleHost::notifyStateChanged(ctkDicomWG23::State state){
    qDebug()<< "new state received:"<< ctkDicomSoapState::toStringValue(state);
    emit stateChangedReceived(state);
}

void ctkDicomExampleHost::notifyStatus(const ctkDicomWG23::Status& status){
    qDebug()<< "new status received:"<<status.codeMeaning;
    emit statusReceived(status);;
}

ctkDicomExampleHost::~ctkDicomExampleHost()
{
  qDebug() << "Exiting host: trying to terminate app";
  this->appProcess.terminate();
  qDebug() << "Exiting host: trying to kill app";
  this->appProcess.kill();
}
