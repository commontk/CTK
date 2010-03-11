#ifndef __qCTKDCMTKCommandWidget_h
#define __qCTKDCMTKCommandWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKCommandWidgetPrivate; 

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKCommandWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKCommandWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKCommandWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKCommandWidget);
};

#endif
