#ifndef __ctkDICOMServerNodeWidget_h
#define __ctkDICOMServerNodeWidget_h

// QT includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMServerNodeWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMServerNodeWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMServerNodeWidget(QWidget* parent=0);
  virtual ~ctkDICOMServerNodeWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMServerNodeWidget);
};

#endif
