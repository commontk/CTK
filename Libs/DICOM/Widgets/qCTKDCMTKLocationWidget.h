#ifndef __qCTKDCMTKLocationWidget_h
#define __qCTKDCMTKLocationWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKLocationWidgetPrivate; 

class CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKLocationWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKLocationWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKLocationWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKLocationWidget);
};

#endif
