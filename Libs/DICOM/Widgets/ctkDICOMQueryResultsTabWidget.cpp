
// Qt widgets
#include <QDebug>
#include <QTabBar>

// ctkDICOMWidgets includes
#include "ctkDICOMQueryResultsTabWidget.h"

//----------------------------------------------------------------------------
// ctkDICOMQueryResultsTabWidget methods

//----------------------------------------------------------------------------
ctkDICOMQueryResultsTabWidget::ctkDICOMQueryResultsTabWidget(QWidget* _parent):Superclass(_parent)
{
}

//----------------------------------------------------------------------------
ctkDICOMQueryResultsTabWidget::~ctkDICOMQueryResultsTabWidget()
{
}


//----------------------------------------------------------------------------
void ctkDICOMQueryResultsTabWidget::disableCloseOnTab(int index)
{
  Q_UNUSED(index);
  this->tabBar()->setTabButton(0, QTabBar::RightSide, 0);
  this->tabBar()->setTabButton(0, QTabBar::LeftSide, 0);
}
