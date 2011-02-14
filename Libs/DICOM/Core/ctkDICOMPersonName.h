#ifndef __ctkDICOMPersonName_h
#define __ctkDICOMPersonName_h

#include "ctkDICOMCoreExport.h"

#include <QString>
#include <QSharedDataPointer>
#include <QMetaType>

#include <string>

class ctkDICOMPersonNameData;
/**
  \brief A person's name as modelled in DICOM.
*/
class CTK_DICOM_CORE_EXPORT ctkDICOMPersonName
{
public:

  ctkDICOMPersonName(const QString& lastName = QString::null,
              const QString& firstName = QString::null,
              const QString& middleName = QString::null,
              const QString& namePrefix = QString::null,
              const QString& nameSuffix = QString::null);

  ctkDICOMPersonName(const ctkDICOMPersonName& other);
  ctkDICOMPersonName& operator=(const ctkDICOMPersonName& other);

  virtual ~ctkDICOMPersonName();
  /**
    \brief "Lastname, FirstName MiddleName, Suffix" (useful for alphabetical sorting)
  */
  QString formattedName() const;

  QString lastName() const;
  QString firstName() const;
  QString middleName() const;
  QString namePrefix() const;
  QString nameSuffix() const;

  /// cast operator
  operator QString() const;
  std::string toStdString() const;

private:
  QSharedDataPointer<ctkDICOMPersonNameData> d;
};

typedef QList<ctkDICOMPersonName> ctkDICOMPersonNameList;
Q_DECLARE_METATYPE(ctkDICOMPersonName);


#endif // ctkDICOMPersonName_h
