

// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

// STD includes
#include <iostream>

//----------------------------------------------------------------------------
class ctkDICOMQueryWidgetPrivate: public Ui_ctkDICOMQueryWidget
{
public:
  ctkDICOMQueryWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMQueryWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::ctkDICOMQueryWidget(QWidget* _parent):Superclass(_parent), 
  d_ptr(new ctkDICOMQueryWidgetPrivate)
{
  Q_D(ctkDICOMQueryWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
}


//----------------------------------------------------------------------------
QMap<QString,QVariant> ctkDICOMQueryWidget::parameters()
{
  Q_D(ctkDICOMQueryWidget);

  QMap<QString,QVariant> parameters;

  parameters["Name"] = d->nameSearch->text();
  parameters["Study"] = d->studySearch->text();
  parameters["Series"] = d->seriesSearch->text();
  parameters["ID"] = d->idSearch->text();

  return parameters;
}
