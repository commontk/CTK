#ifndef __ctkDICOMQueryRetrieveWidget_h
#define __ctkDICOMQueryRetrieveWidget_h

// Qt includes 
#include <QWidget>

// CTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMQueryRetrieveWidgetPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMQueryRetrieveWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit ctkDICOMQueryRetrieveWidget(QWidget* parent=0);
  virtual ~ctkDICOMQueryRetrieveWidget();

private:
  CTK_DECLARE_PRIVATE(ctkDICOMQueryRetrieveWidget);
};

#endif
