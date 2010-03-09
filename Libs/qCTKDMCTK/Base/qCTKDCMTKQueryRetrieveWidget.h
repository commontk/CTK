#ifndef __qCTKDCMTKQueryRetrieveWidget_h
#define __qCTKDCMTKQueryRetrieveWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <qCTKPimpl.h>

#include "qCTKDMCTKExport.h"

class qCTKDCMTKQueryRetrieveWidgetPrivate; 

class Q_CTKDMCTK_EXPORT qCTKDCMTKQueryRetrieveWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKQueryRetrieveWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKQueryRetrieveWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKQueryRetrieveWidget);
};

#endif
