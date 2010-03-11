#ifndef __qCTKDCMTKServerNodeWidget_h
#define __qCTKDCMTKServerNodeWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKServerNodeWidgetPrivate; 

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKServerNodeWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKServerNodeWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKServerNodeWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKServerNodeWidget);
};

#endif
