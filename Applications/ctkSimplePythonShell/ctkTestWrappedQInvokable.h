

#ifndef __ctkTestWrappedQInvokable_h
#define __ctkTestWrappedQInvokable_h

// Qt includes
#include <QObject>

class ctkTestWrappedQInvokable : public QObject
{
  Q_OBJECT
public:

  ctkTestWrappedQInvokable(QObject * newParent = 0) : QObject(newParent)
    {
    this->Value = 0;
    }

  /// Example of method wrapped using Q_INVOKABLE
  Q_INVOKABLE int value() const { return this->Value; }
  Q_INVOKABLE void setValue(int newValue){ this->Value = newValue; }

private:
  int        Value;
};

#endif
