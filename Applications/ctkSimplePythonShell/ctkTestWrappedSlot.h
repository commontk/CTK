

#ifndef __ctkTestWrappedSlot_h
#define __ctkTestWrappedSlot_h

// Qt includes
#include <QObject>

class ctkTestWrappedSlot : public QObject
{
  Q_OBJECT

public:

  ctkTestWrappedSlot(QObject * newParent = 0) : QObject(newParent)
    {
    this->Value = 0;
    }

public slots:

  int value() const { return this->Value; }
  void setValue(int newValue){ this->Value = newValue; }

private:
  int        Value;
};

#endif
