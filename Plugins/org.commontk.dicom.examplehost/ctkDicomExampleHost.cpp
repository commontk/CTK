#include "ctkDicomExampleHost.h"
#include <QProcess>
#include <QtDebug>

ctkDicomExampleHost::ctkDicomExampleHost(QWidget* placeholderWidget) : placeholderWidget(placeholderWidget),ctkDicomAbstractHost(8080)
{
}

void ctkDicomExampleHost::StartApplication(QString AppPath, const QUrl& App_URL){

    QStringList l;
    l.append("--hostURL");
    l.append(QString("http://localhost:") + QString::number(this->getPort()) );
    l.append("--applicationURL");
    l.append(App_URL.toString());
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
    qDebug()<< "new state received:"<< static_cast<int>(state);
    emit stateChangedReceived(state);
}

void ctkDicomExampleHost::notifyStatus(const ctkDicomWG23::Status& status){
    qDebug()<< "new status received:"<<status.codeMeaning;
    emit statusReceived(status);;
}

ctkDicomExampleHost::~ctkDicomExampleHost()
{
  this->appProcess.terminate();
}
