#include "ctkHostAppExampleWidget.h"
#include "ui_ctkHostAppExampleWidget.h"

#include <QDebug>

ctkHostAppExampleWidget::ctkHostAppExampleWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ctkHostAppExampleWidget)
{
    qDebug() << "setup ui";
    ui->setupUi(this);
}

ctkHostAppExampleWidget::~ctkHostAppExampleWidget()
{
    delete ui;
}

void ctkHostAppExampleWidget::startButtonClicked()
{
    qDebug() << "start button clicked";
}

void ctkHostAppExampleWidget::stopButtonClicked()
{
    qDebug() << "stop button clicked";
}
