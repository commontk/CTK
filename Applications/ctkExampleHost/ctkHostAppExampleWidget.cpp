#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"
#include "ctkDicomExampleHost.h"

#include <QDebug>
#include <QFileDialog>
#include <QProcess>

ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
    qDebug() << "setup ui";
    ui->setupUi(this);
    ui->crashLabel->setVisible(false);
    ui->messageOutput->setVisible(false);
    this->host = new ctkDicomExampleHost();
    connect(&this->host->getAppProcess(),SIGNAL(error(QProcess::ProcessError)),SLOT(appProcessError(QProcess::ProcessError)));
    connect(&this->host->getAppProcess(),SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(appProcessStateChanged(QProcess::ProcessState)));
  }


ctkHostAppExampleWidget::~ctkHostAppExampleWidget()
{
    delete ui;
}

void ctkHostAppExampleWidget::startButtonClicked()
{
    qDebug() << "start button clicked";
    if (host)
    {
      host->StartApplication(appFileName);
    }
}

void ctkHostAppExampleWidget::stopButtonClicked()
{
    qDebug() << "stop button clicked";
}

void ctkHostAppExampleWidget::loadButtonClicked()
{
    qDebug() << "load button clicked";
    this->appFileName = QFileDialog::getOpenFileName(this,"Choose hosted application",QApplication::applicationDirPath());
    if (QFile(this->appFileName).permissions() & QFile::ExeUser )
    {
     this->ui->applicationPathLabel->setText(this->appFileName);
    }
    else
    {
      this->ui->applicationPathLabel->setText(QString("<font color='red'>Not executable:</font>").append(this->appFileName));
    }
}
void ctkHostAppExampleWidget::appProcessError(QProcess::ProcessError error)
{
  if (error == QProcess::Crashed)
  {
    qDebug() << "crash detected";
    ui->crashLabel->setVisible(true);
  }
  }

void ctkHostAppExampleWidget::appProcessStateChanged(QProcess::ProcessState state)
{
  QString labelText;
  switch (state){
  case QProcess::Running:
    ui->processStateLabel->setText("Running");
    break;
  case QProcess::NotRunning:
    if (host->getAppProcess().exitStatus() == QProcess::CrashExit )
    {
        labelText = "crashed";
    }
    else
    {
      labelText = "Not Running, last exit code ";
      labelText.append(QString::number(host->getAppProcess().exitCode()));
    }
    ui->processStateLabel->setText(labelText);
    break;
  case QProcess::Starting:
    ui->processStateLabel->setText("Starting");
    break;
  default:
    ;
  }
  }
