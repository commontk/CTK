

#ifndef __ctkTestWrappedQProperty_h
#define __ctkTestWrappedQProperty_h

// Qt includes
#include <QObject>

class ctkTestWrappedQProperty : public QObject
{
  Q_OBJECT

  Q_PROPERTY(int value READ value WRITE setValue);

public:

  ctkTestWrappedQProperty(QObject * newParent = 0) : QObject(newParent)
    {
    this->Value = 0;
    }

  /// Example of property declared using Q_PROPERTY
  /// Using Q_PROPERTY is enough to expose them, it's not required to declare them as slot
  int value() const { return this->Value; }
  void setValue(int newValue){ this->Value = newValue; }

private:
  int        Value;
};

#endif
