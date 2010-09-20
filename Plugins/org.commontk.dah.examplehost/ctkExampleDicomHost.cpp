#include "ctkExampleDicomHost.h"
#include "ctkDicomAppHostingTypesHelper.h"

#include <QProcess>
#include <QtDebug>
#include <QRect>

#include <iostream>

ctkExampleDicomHost::ctkExampleDicomHost(ctkHostedAppPlaceholderWidget* placeholderWidget, int hostPort, int appPort) :
    ctkDicomAbstractHost(hostPort, appPort),
    placeholderWidget(placeholderWidget),
    applicationState(ctkDicomAppHosting::IDLE)
{
  connect(&this->appProcess,SIGNAL(readyReadStandardOutput()),SLOT(forwardConsoleOutput()));
}

void ctkExampleDicomHost::StartApplication(QString AppPath){

    QStringList l;
    l.append("--hostURL");
    l.append(QString("http://localhost:") + QString::number(this->getHostPort()) + "/HostInterface" );
    l.append("--applicationURL");
    l.append(QString("http://localhost:") + QString::number(this->getAppPort()) + "/ApplicationInterface" );
    //by default, the ctkExampleHostedApp uses the org.commontk.dah.exampleapp plugin
    //l.append("dicomapp"); // the app plugin to use - has to be changed later
    //if (!QProcess::startDetached (
    //{
    //    qCritical() << "application failed to start!";
    //}
    //qDebug() << "starting application: " << AppPath << " " << l;
    qDebug() << "starting application: " << AppPath << " " << l;
    this->appProcess.setProcessChannelMode(QProcess::MergedChannels);
    this->appProcess.start(AppPath,l);
}

QRect ctkExampleDicomHost::getAvailableScreen(const QRect& preferredScreen){
    qDebug()<< "Application asked for this area:"<< preferredScreen;

    QRect rect (this->placeholderWidget->getAbsolutePosition());

    emit giveAvailableScreen(rect);
    return rect;
}


void ctkExampleDicomHost::notifyStateChanged(ctkDicomAppHosting::State state){
    qDebug()<< "new state received:"<< static_cast<int>(state);
    qDebug()<< "new state received:"<< ctkDicomSoapState::toStringValue(state);
    emit stateChangedReceived(state);
}

void ctkExampleDicomHost::notifyStatus(const ctkDicomAppHosting::Status& status){
    qDebug()<< "new status received:"<<status.codeMeaning;
    emit statusReceived(status);;
}

ctkExampleDicomHost::~ctkExampleDicomHost()
{
  qDebug() << "Exiting host: trying to terminate app";
  this->appProcess.terminate();
  qDebug() << "Exiting host: trying to kill app";
  this->appProcess.kill();
}

void ctkExampleDicomHost::forwardConsoleOutput()
{
  while( this->appProcess.bytesAvailable() )
  {
    QString line( this->appProcess.readLine() );
    line.prepend(">>>> ");
    std::cout << line.toStdString();
  }
}

bool ctkExampleDicomHost::notifyDataAvailable(ctkDicomAppHosting::AvailableData data, bool lastData)
{
  Q_UNUSED(data)
  Q_UNUSED(lastData)
  return false;
}

QList<ctkDicomAppHosting::ObjectLocator>* ctkExampleDicomHost::getData(
  QList<QUuid> objectUUIDs, 
  QList<QString> acceptableTransferSyntaxUIDs, 
  bool includeBulkData)
{
  Q_UNUSED(objectUUIDs)
  Q_UNUSED(acceptableTransferSyntaxUIDs)
  Q_UNUSED(includeBulkData)
  return NULL;
}
void ctkExampleDicomHost::releaseData(QList<QUuid> objectUUIDs)
{
  Q_UNUSED(objectUUIDs)
}
