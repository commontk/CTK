#ifndef CTKEVENTBUSDEMOMAINWINDOW_H
#define CTKEVENTBUSDEMOMAINWINDOW_H

#include <QMainWindow>
#include <QVariant>

class ctkEventAdminBus;

namespace Ui {
    class ctkEventBusDemoMainWindow;
}

class ctkEventDemo : public QObject {
    Q_OBJECT

Q_SIGNALS:
    void receiveEventSignal(QVariantList l);
    void updateMessageSignal(QString message);

public Q_SLOTS:
    void receiveEvent(QVariantList l);
};

class ctkEventBusDemoMainWindow : public QMainWindow {
    Q_OBJECT
public:
    ctkEventBusDemoMainWindow(QWidget *parent = 0);
    ctkEventBusDemoMainWindow(ctkEventAdminBus *bus, QWidget *parent = 0);
    ~ctkEventBusDemoMainWindow();

public Q_SLOTS:
    void sendEvent();
    void updateMessage(QString message);
    void connectClient();

protected:
    void changeEvent(QEvent *e);
    void connectEvents();

private:
    Ui::ctkEventBusDemoMainWindow *ui;
    ctkEventAdminBus *m_EventBus;

    ctkEventDemo *handler;
};

#endif // CTKEVENTBUSDEMOMAINWINDOW_H
