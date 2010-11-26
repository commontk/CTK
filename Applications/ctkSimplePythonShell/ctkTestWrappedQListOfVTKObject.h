

#ifndef __ctkTestWrappedQListOfVTKObject_h
#define __ctkTestWrappedQListOfVTKObject_h

// Qt includes
#include <QObject>
#include <QList>

// VTK includes
#include <vtkTable.h>

class ctkTestWrappedQListOfVTKObject : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedQListOfVTKObject(QObject * newParent = 0) : QObject(newParent)
    {
    }

  /// Example ot slot accepting a VTK object as parameter
  Q_INVOKABLE int numberOfElementInList(const QList<vtkTable*>& listOfTable)
    {
    return listOfTable.count();
    }
};

#endif
