#ifndef __qCTKDCMTKListenerWidget_h
#define __qCTKDCMTKListenerWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKListenerWidgetPrivate; 

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKListenerWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKListenerWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKListenerWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKListenerWidget);
};

#endif
