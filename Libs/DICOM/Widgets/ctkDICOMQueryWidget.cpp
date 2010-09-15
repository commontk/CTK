

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
void ctkDICOMQueryWidget::populateQuery(/*ctkDICOMQuery &query*/)
{
  Q_D(ctkDICOMQueryWidget);
  if ( d->CT->isChecked() )
  {
    std::cerr << "CT\n";
    //query.addModality( "CT" );
  }

  if ( d->MR->isChecked() )
  {
    std::cerr << "MR\n";
    //query.addModality( "CT" );
  }

}
