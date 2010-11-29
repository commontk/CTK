

#ifndef __ctkTestWrappedVTKSlot_h
#define __ctkTestWrappedVTKSlot_h

// Qt includes
#include <QObject>

// VTK includes
#include <vtkTable.h>

class ctkTestWrappedVTKSlot : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedVTKSlot(QObject * newParent = 0) : QObject(newParent)
    {
    this->MyTable = vtkTable::New();
    }
    
  virtual ~ctkTestWrappedVTKSlot()
    {
    this->MyTable->Delete();
    }

public slots:

  /// Example of slot returning a VTK object
  vtkTable* getTable() const
    {
    return this->MyTable;
    }

  /// Example ot slot accepting a VTK object as parameter
  void setTable(vtkTable * newTable)
    {
    this->MyTable = newTable;
    }

private:
  vtkTable * MyTable;
};

#endif
