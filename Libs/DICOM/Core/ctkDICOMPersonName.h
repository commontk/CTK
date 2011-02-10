#ifndef CTKDICOMPERSONNAME_H
#define CTKDICOMPERSONNAME_H

#include "ctkDICOMCoreExport.h"

#include <QString>
#include <QSharedDataPointer>
#include <QMetaType>

#include <string>

class QPersonNameData;
/**
  \brief A person's name as modelled in DICOM.
*/
class CTK_DICOM_CORE_EXPORT QPersonName
{

  public:

    QPersonName(const QString& lastName = QString::null,
                const QString& firstName = QString::null,
                const QString& middleName = QString::null,
                const QString& namePrefix = QString::null,
                const QString& nameSuffix = QString::null);

    QPersonName(const QPersonName& other);
    QPersonName& operator=(const QPersonName& other);

    virtual ~QPersonName();
    /**
      \brief "Lastname, FirstName MiddleName, Suffix" (useful for alphabetical sorting)
    */
    QString GetFormattedName() const;

    QString GetLastName() const;
    QString GetFirstName() const;
    QString GetMiddleName() const;
    QString GetNamePrefix() const;
    QString GetNameSuffix() const;

    /// cast operator
    operator QString() const;
    std::string toStdString() const;

  private:
    QSharedDataPointer<QPersonNameData> d;
};

typedef QList<QPersonName> QPersonNameList;
Q_DECLARE_METATYPE(QPersonName);


#endif // CTKDICOMPERSONNAME_H
