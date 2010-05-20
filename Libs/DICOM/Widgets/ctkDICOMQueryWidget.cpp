#include <iostream>

// ctkDICOMWidgets includes
#include "ctkDICOMQueryWidget.h"
#include "ui_ctkDICOMQueryWidget.h"

//----------------------------------------------------------------------------
class ctkDICOMQueryWidgetPrivate: public ctkPrivate<ctkDICOMQueryWidget>,
                                  public Ui_ctkDICOMQueryWidget
{
public:
  ctkDICOMQueryWidgetPrivate(){}
};

//----------------------------------------------------------------------------
// ctkDICOMQueryWidgetPrivate methods


//----------------------------------------------------------------------------
// ctkDICOMQueryWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::ctkDICOMQueryWidget(QWidget* _parent):Superclass(_parent)
{
  CTK_INIT_PRIVATE(ctkDICOMQueryWidget);
  CTK_D(ctkDICOMQueryWidget);
  
  d->setupUi(this);
}

//----------------------------------------------------------------------------
ctkDICOMQueryWidget::~ctkDICOMQueryWidget()
{
}


//----------------------------------------------------------------------------
void ctkDICOMQueryWidget::populateQuery(/*ctkDICOMQuery &query*/)
{
  CTK_D(ctkDICOMQueryWidget);
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
