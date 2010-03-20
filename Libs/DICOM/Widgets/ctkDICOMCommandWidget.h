#ifndef __ctkDICOMCommandWidget_h
#define __ctkDICOMCommandWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMCommandWidgetPrivate; 

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMCommandWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMCommandWidget(QWidget* parent=0);
  virtual ~ctkDICOMCommandWidget();

private:
  QCTK_DECLARE_PRIVATE(ctkDICOMCommandWidget);
};

#endif
