#ifndef CTKHOSTAPPEXAMPLEWIDGET_H
#define CTKHOSTAPPEXAMPLEWIDGET_H

#include <QWidget>
#include <QProcess>

#include <ctkDicomWG23Types.h>

class ctkExampleDicomHost;

namespace Ui {
    class ctkHostAppExampleWidget;
}

class ctkHostAppExampleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ctkHostAppExampleWidget(QWidget *parent = 0);
    void setAppFileName(QString name);

    virtual ~ctkHostAppExampleWidget();
public slots:
    void loadButtonClicked();
    void startButtonClicked();
    void runButtonClicked();
    void stopButtonClicked();
    void appProcessError(QProcess::ProcessError error);
    void appProcessStateChanged(QProcess::ProcessState state);
    void appStateChanged(ctkDicomWG23::State state);

    void placeholderResized();

protected:
    ctkExampleDicomHost* host;
    QString appFileName;

private:
    Ui::ctkHostAppExampleWidget *ui;

};

#endif // CTKHOSTWIDGET_H
