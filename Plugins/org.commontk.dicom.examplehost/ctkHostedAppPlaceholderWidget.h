#ifndef CTKHOSTEDAPPPLACEHOLDERWIDGET_H
#define CTKHOSTEDAPPPLACEHOLDERWIDGET_H

#include <QFrame>
#include <QDebug>
#include <org_commontk_dicom_examplehost_Export.h>

class org_commontk_dicom_examplehost_EXPORT ctkHostedAppPlaceholderWidget : public QFrame
{
  Q_OBJECT
public:
    explicit ctkHostedAppPlaceholderWidget(QWidget *parent = 0);
    QRect getAbsolutePosition()
    {
      QWidget* current = this;
      int x = 0;
      int y = 0;
      do
      {
        x = x + current->x();
        y = y + current->y();
        current = dynamic_cast<QWidget*>(current->parent());
      } while (current);
      return QRect(x,y,width(),height());
    }

signals:
  void resized();
protected:
  void resizeEvent(QResizeEvent* /* event */) { emit resized(); }
};

#endif // CTKHOSTEDAPPPLACEHOLDERWIDGET_H
