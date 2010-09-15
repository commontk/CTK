#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"
#include "ctkDicomExampleHost.h"

#include <QDebug>
#include <QFileDialog>

ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
    qDebug() << "setup ui";
    ui->setupUi(this);
    this->host = new ctkDicomExampleHost();

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
      // host->StartApplication(appFileName);
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
