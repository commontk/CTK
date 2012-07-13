#include "ctkEventBusDemoMainWindow.h"
#include "ui_ctkEventBusDemoMainWindow.h"

#include "ctkEventAdminBus.h"

ctkEventBusDemoMainWindow::ctkEventBusDemoMainWindow(QWidget *parent)
  :  QMainWindow(parent), ui(new Ui::ctkEventBusDemoMainWindow)
{
    ui->setupUi(this);
}

ctkEventBusDemoMainWindow::ctkEventBusDemoMainWindow(ctkEventAdminBus *bus, QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::ctkEventBusDemoMainWindow),  m_EventBus(bus)
{
    ui->setupUi(this);
    connectEvents();
}


ctkEventBusDemoMainWindow::~ctkEventBusDemoMainWindow()
{
    delete handler;
    delete ui;
}

void ctkEventBusDemoMainWindow::connectEvents() {
    handler = new ctkEventDemo();
    connect(ui->btnSend, SIGNAL(released()), this, SLOT(sendEvent()));
    connect(handler, SIGNAL(updateMessageSignal(QString)), this, SLOT(updateMessage(QString)));
    connect(ui->connectButton, SIGNAL(released()), this, SLOT(connectClient()));

    qDebug() << "connectEvents";
    m_EventBus->publishSignal(handler, "receiveEventSignal(QVariantList)", "ctk/remote/eventBus/comunication/receive/xmlrpc");
    ctkDictionary dic;
    dic.insert("event.topics","ctk/remote/eventBus/comunication/receive/xmlrpc");
    m_EventBus->subscribeSlot(handler, "receiveEvent(QVariantList)", dic);
}

void ctkEventBusDemoMainWindow::sendEvent() {

    QString textToDisplay("Me: ");
    textToDisplay.append(ui->txtParameter->property("plainText").toString());
    ui->textBrowser->append(textToDisplay);

    // event bus starts here

    QVariantList localEventList;
    localEventList.append("ctk/remote/eventBus/comunication/receive/xmlrpc");

    QVariantList dataList;
    dataList.append("myUser");
    dataList.append(ui->txtParameter->property("plainText").toString());

    ctkDictionary dic;

    dic.insert("localEvent",localEventList);
    dic.insert("localData",dataList);

    QString value = "ctk/remote/eventBus/comunication/send/xmlrpc";
    ctkEvent event(value,dic);

    m_EventBus->sendEvent(event);
}

void ctkEventBusDemoMainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void ctkEventBusDemoMainWindow::updateMessage(QString message) {
    ui->textBrowser->append(message);
}

void ctkEventBusDemoMainWindow::connectClient() {
    bool result, resultClient, resultServer;
    resultClient = m_EventBus->createServer("XMLRPC", ui->portLineEdit->text().toInt());
    m_EventBus->startListen();
    resultServer = m_EventBus->createClient("XMLRPC", ui->hostLineEdit->text(), ui->portLineEdit->text().toInt());
    result = resultClient && resultServer;
    if(result) {
        ui->hostLineEdit->setEnabled(false);
        ui->portLineEdit->setEnabled(false);
        ui->connectButton->setEnabled(false);

        ui->txtParameter->setEnabled(true);
        ui->btnSend->setEnabled(true);
    }
}

void ctkEventDemo::receiveEvent(QVariantList l) {
    QString value;
    value.append(l.at(0).toString());
    value.append(": ");
    value.append(l.at(1).toString());
    emit updateMessageSignal(value);
}
