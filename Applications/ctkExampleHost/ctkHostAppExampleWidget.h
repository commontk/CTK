#ifndef CTKHOSTAPPEXAMPLEWIDGET_H
#define CTKHOSTAPPEXAMPLEWIDGET_H

#include <QWidget>

class ctkDicomExampleHost;

namespace Ui {
    class ctkHostAppExampleWidget;
}

class ctkHostAppExampleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ctkHostAppExampleWidget(QWidget *parent = 0);
    ~ctkHostAppExampleWidget();
public slots:
    void startButtonClicked();
    void stopButtonClicked();
    void loadButtonClicked();
protected:
    ctkDicomExampleHost* host;
    QString appFileName;

private:
    Ui::ctkHostAppExampleWidget *ui;

};

#endif // CTKHOSTWIDGET_H
