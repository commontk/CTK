#ifndef __qCTKDCMTKQueryRetrieveWidget_h
#define __qCTKDCMTKQueryRetrieveWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKQueryRetrieveWidgetPrivate; 

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKQueryRetrieveWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKQueryRetrieveWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKQueryRetrieveWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKQueryRetrieveWidget);
};

#endif
