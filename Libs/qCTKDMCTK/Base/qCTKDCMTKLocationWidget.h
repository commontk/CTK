#ifndef __qCTKDCMTKLocationWidget_h
#define __qCTKDCMTKLocationWidget_h

// QT includes 
#include <QWidget>

// qCTK includes
#include <qCTKPimpl.h>

#include "qCTKDMCTKExport.h"

class qCTKDCMTKLocationWidgetPrivate; 

class Q_CTKDMCTK_EXPORT qCTKDCMTKLocationWidget : public QWidget
{
public:
  typedef QWidget Superclass;
  explicit qCTKDCMTKLocationWidget(QWidget* parent=0);
  virtual ~qCTKDCMTKLocationWidget();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKLocationWidget);
};

#endif
