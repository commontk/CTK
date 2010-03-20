#ifndef __ctkDICOMQueryWidget_h
#define __ctkDICOMQueryWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMQueryWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMQueryWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryWidget();

private:
  QCTK_DECLARE_PRIVATE(ctkDICOMQueryWidget);
};

#endif
