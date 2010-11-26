#ifndef __ctkTestWrappedVTKQInvokable_h
#define __ctkTestWrappedVTKQInvokable_h

// Qt includes
#include <QObject>

// VTK includes
#include <vtkTable.h>

class ctkTestWrappedVTKQInvokable : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedVTKQInvokable(QObject * newParent = 0) : QObject(newParent)
    {
    this->MyTable = vtkTable::New();
    }
    
  virtual ~ctkTestWrappedVTKQInvokable()
    {
    this->MyTable->Delete();
    }

  /// Example of 'invokable' returning a VTK object
  /// Declaring a method as invokable allows to add it to the MetaObject system
  /// \note When a method returns a value, we tend to use Q_INVOKABLE
  /// instead of declaring a slot.
  Q_INVOKABLE vtkTable * getTable() const
    {
    return this->MyTable;
    }

  /// Example of 'invokable' accepting a VTK object as parameter
  Q_INVOKABLE void setTable(vtkTable * newTable)
    {
    this->MyTable = newTable;
    }

private:
  vtkTable * MyTable;
};

#endif
