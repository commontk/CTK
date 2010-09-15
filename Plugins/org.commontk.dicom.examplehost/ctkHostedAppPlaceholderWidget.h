#ifndef CTKHOSTEDAPPPLACEHOLDERWIDGET_H
#define CTKHOSTEDAPPPLACEHOLDERWIDGET_H

#include <QFrame>
#include <org_commontk_dicom_examplehost_Export.h>

class org_commontk_dicom_examplehost_EXPORT ctkHostedAppPlaceholderWidget : public QFrame
{
  Q_OBJECT
public:
    explicit ctkHostedAppPlaceholderWidget(QWidget *parent = 0);
signals:
  void resized();
protected:
  void resizeEvent(QResizeEvent* event) { emit resized(); }
};

#endif // CTKHOSTEDAPPPLACEHOLDERWIDGET_H
