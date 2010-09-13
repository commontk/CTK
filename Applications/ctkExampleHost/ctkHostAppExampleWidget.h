#ifndef CTKHOSTAPPEXAMPLEWIDGET_H
#define CTKHOSTAPPEXAMPLEWIDGET_H

#include <QDockWidget>

namespace Ui {
    class ctkHostAppExampleWidget;
}

class ctkHostAppExampleWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit ctkHostAppExampleWidget(QWidget *parent = 0);
    ~ctkHostAppExampleWidget();
public slots:
    void startButtonClicked();
    void stopButtonClicked();

private:
    Ui::ctkHostAppExampleWidget *ui;
};

#endif // CTKHOSTWIDGET_H
