#include <QSharedData>

#include "ctkDICOMPersonName.h"


// ------------------------------------------- QPersonName -----------------------------------------------------------

class QPersonNameData : public QSharedData
{
  public:

    QString m_LastName;
    QString m_FirstName;
    QString m_MiddleName;
    QString m_NamePrefix;
    QString m_NameSuffix;
};

QPersonName::QPersonName(const QString& lastName,
                         const QString& firstName,
                         const QString& middleName,
                         const QString& namePrefix,
                         const QString& nameSuffix)
:d(new QPersonNameData)
{
  d->m_LastName = lastName;
  d->m_FirstName = firstName;
  d->m_MiddleName = middleName;
  d->m_NamePrefix = namePrefix;
  d->m_NameSuffix = nameSuffix;
}

QPersonName::QPersonName(const QPersonName& other) : d(other.d)
{
}

QPersonName& QPersonName::operator=(const QPersonName& other)
{
  d=other.d;
  return *this;
}

QPersonName::~QPersonName()
{
}


QString QPersonName::GetFormattedName() const
{
  QString result("");

  /* not sortable
  if (!m_NamePrefix.isEmpty())  result += QString("%1 ").arg(m_NamePrefix);
  if (!m_FirstName.isEmpty())  result += QString("%1 " ).arg(m_FirstName);
  if (!m_MiddleName.isEmpty()) result += QString("%1 ").arg(m_MiddleName);
  if (!m_LastName.isEmpty())   result += QString("%1").arg(m_LastName);
  if (!m_NameSuffix.isEmpty()) result += QString(", %1").arg(m_NameSuffix); // this might be unclean if last name is empty
  */

  if (!d->m_LastName.isEmpty())   result += QString("%1").arg(d->m_LastName);
  if (!d->m_FirstName.isEmpty())  result += QString(", %1" ).arg(d->m_FirstName);
  if (!d->m_MiddleName.isEmpty()) result += QString(" %1").arg(d->m_MiddleName);
  if (!d->m_NameSuffix.isEmpty()) result += QString(", %1").arg(d->m_NameSuffix); // this might be unclean if last name is empty

  return result;
}

QString QPersonName::GetLastName()  const
{
  return d->m_LastName;
}

QString QPersonName::GetFirstName()  const
{
  return d->m_FirstName;
}

QString QPersonName::GetMiddleName()  const
{
  return d->m_MiddleName;
}

QString QPersonName::GetNamePrefix()  const
{
  return d->m_NamePrefix;
}

QString QPersonName::GetNameSuffix()  const
{
  return d->m_NameSuffix;
}

QPersonName::operator QString() const
{
  return this->GetFormattedName();
}

std::string QPersonName::toStdString() const
{
  // the complicated looking .toLocal8Bit().constData() is on purpose.
  // if we'd use .toStdString(), the string would be converted to ASCII
  // instead of the local 8bit character encoding.
  // changes for correctly looking output of the strings are higher with .toLocal8Bit()
  return this->GetFormattedName().toLocal8Bit().constData();
}

