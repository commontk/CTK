#ifndef __qCTKDCMTKQueryWidget_h
#define __qCTKDCMTKQueryWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKQueryWidgetPrivate; 

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKQueryWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKQueryWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKQueryWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKQueryWidget);
};

#endif
