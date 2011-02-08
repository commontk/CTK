/*=========================================================================

  Library:   CTK

  Copyright (c) Mint Medical GmbH

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

#ifndef __ctkDICOMDataset_h
#define __ctkDICOMDataset_h

#include "ctkDICOMCoreExport.h"

#ifndef WIN32
  #define HAVE_CONFIG_H
#endif
#include <dcdatset.h> // DCMTK DcmDataset

#include <QtCore>

class DcmDataDictionary;

class QPersonNamePrivate;
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
    QScopedPointer<QPersonNamePrivate> d_ptr;
    Q_DECLARE_PRIVATE(QPersonName);
};

typedef QList<QPersonName> QPersonNameList;
Q_DECLARE_METATYPE(QPersonName);

class ctkDICOMDatasetPrivate;
/**
  \brief Base class for DICOM objects and interface with DICOM toolkit DCMTK.

  This class serves as the base class for all DICOM objects (patient, study, series, image).

  The class is derived from DcmDataset, the data type that is used by the DICOM toolkit when
  reading an image file or formulating a message request or receiving a message response (e.g. C-FIND).

  Basically it offers a lot of convenience methods for subclasses to read and write DICOM attributes
  using Qt types. It is the subclasses responsibility to use the correct data types as defined in DICOM.

  \note ONLY the Get.. methods should be used to access the internal dataset.

  When reading string type attributes (LO, LT, PN, SH, ST, UT), we consider the "specific character set"
  tag and decode the stored string using the correct encoding (using Qt methods). This allows to
  display e.g. person names written in arabic, hebrew, greek, russian, etc. letters.

  \warning Right now, asian phonetic strings cause problems. We have to learn the concept.
  \warning Helpers for writing DICOM attributes are not yet implemented. Implementation is straightforward though and can be done when necessary.
  \warning DateTime objects ignore the timezone at the moment. This is however of secondary importance.

  A subclass could possibly want to store the internal DcmDataset.
  For this purpose, the internal DcmDataset is serialized into a memory buffer using DcmDataset::write(..). This buffer
  is stored in a base64 encoded string. For deserialization we decode the string and use DcmDataset::read(..).
*/
class CTK_DICOM_CORE_EXPORT ctkDICOMDataset : public DcmDataset
{
public:
    typedef QObject Superclass;
    ctkDICOMDataset();
    virtual ~ctkDICOMDataset();

    /**
      \brief For initialization from a DcmDataset in a constructor / assignment.

      This method should be overwritten by all derived classes. It should
      be called from the constructor or assignment operators when the class
      should copy information from a DcmDataset object.

      \warning Derived classes must call PDICOMDataset::InitializeFromDataset(...) to correctly copy encoding information.
    */
    virtual void InitializeFromDataset(DcmDataset* dataset);

    /**
     \brief Store a string representation of the object to a database field.

     The internal DcmDataset is serialized into a memory buffer using DcmDataset::write(..).
     To store the memory buffer in a simple string database field, we convert it to a base64 encoded string.
     Doing so prevents errors from encoding conversions that could be made by QString or the database etc.
    */
    void Serialize();

    /**
     \brief Restore the object from a string representation in a database field.

     The database stored string is base64 decoded into a memory buffer. Then
     the internal DcmDataset is created using DcmDataset::read(..).
    */
    void Deserialize();


    /**
      \brief To be called from InitializeData, flags status as dirty.

      This is to allow data to be read as late as possible. All the
      Get/SetElement... methods ensure initialization, which checks this flag.
    */
    void MarkForInitialization();

    /**
      \brief Called by all Get/Set methods to initialize DcmDataSet if needed.
    */
    void EnsureDcmDataSetIsInitialized() const;


    /**
      \brief Find element in dataset and copy it into internal DcmDataset

      Attribute types 1, 1C, 2, 2C, 3 as defined in DICOM can be encoded as
      hex values 0x1, 0x1C, 0x2, 0x2C, 0x3.

      Conditional attributes are considered MUST attributes. The calling
      function shall test the conditions before calling CopyElement
      (since conditions might be complex).
    */
    bool CopyElement( DcmDataset* dataset, const DcmTagKey& tag, int type );

    /**
      \brief creates a QString from the OFString, respecting the "specific character set" of the Dataset.

      This method checks if the dataset has an attribute "specific character set".
      If so, all attributes of types Long String (LO), Long Text (LT), Person Name (PN), Short String (SH),
      Short Text (ST), Unlimited Text (UT) should be interpreted as encoded with a special set.

      See implementation for details.
    */
    QString Decode(const DcmTag& tag, const OFString& raw) const;

    /**
      \brief creates an OFString from the QtString

      \warning The method currently assumes that the encoding of the passed string if latin1 and converts
      it accordingly. The passed DICOM tag is not yet evaluated to determine the actual encoding type.
    */
    OFString Encode(const DcmTag& tag, const QString& qstring) const;

    /**
      \brief A const-correct version of DcmDataset::findAndGetElement.
    */
    OFCondition findAndGetElement(const DcmTag& tag, DcmElement*& element, const OFBool searchIntoSub=OFFalse) const; // DCMTK is not const-correct

    /**
      \brief A const-correct version of DcmDataset::findAndGetOFString.
    */
    OFCondition findAndGetOFString(const DcmTag& tag, OFString& value, const unsigned long pos = 0, const OFBool searchIntoSub=OFFalse) const; // DCMTK is not const-correct

    static bool CheckCondition(const OFCondition&);

    /**
      \brief Get-methods for for all subtypes of DcmByteString
    */
    QString          GetAllElementValuesAsString( const DcmTag& tag ) const;
    QString                   GetElementAsString( const DcmTag& tag, unsigned long pos = 0 ) const;
    QStringList           GetElementAsStringList( const DcmTag& tag ) const;
    QPersonName           GetElementAsPersonName( const DcmTag& tag, unsigned long pos = 0 ) const;
    QPersonNameList   GetElementAsPersonNameList( const DcmTag& tag ) const;
    QDate                       GetElementAsDate( const DcmTag& tag, unsigned long pos = 0 ) const;
    QTime                       GetElementAsTime( const DcmTag& tag, unsigned long pos = 0 ) const;
    double                    GetElementAsDouble( const DcmTag& tag, unsigned long pos = 0 ) const; // type DS
    long                     GetElementAsInteger( const DcmTag& tag, unsigned long pos = 0 ) const; // type IS
    int                  GetElementAsSignedShort( const DcmTag& tag, unsigned long pos = 0 ) const; // type SS
    int                GetElementAsUnsignedShort( const DcmTag& tag, unsigned long pos = 0 ) const; // type US

    /**
    \warning IGNORES TIME ZONE at the moment!
    */
    QDateTime       GetElementAsDateTime( const DcmTag& tag, unsigned long pos = 0 ) const;


    /**
    \brief Set-methods for for all subtypes of DcmByteString
    */
    bool SetElementAsString( const DcmTag& tag, QString string );
    bool SetElementAsStringList( const DcmTag& tag, QStringList stringList ); //> Currently not implemented
    bool SetElementAsPersonName( const DcmTag& tag, QPersonName personName );
    bool SetElementAsPersonNameList( const DcmTag& tag, QPersonNameList personNameList ); //> Currently not implemented
    bool SetElementAsDate( const DcmTag& tag, QDate date );
    bool SetElementAsTime( const DcmTag& tag, QTime time );
    bool SetElementAsDateTime( const DcmTag& tag, QDateTime dateTime );
    bool SetElementAsInteger( const DcmTag& tag, long value, unsigned long pos = 0 ); // type IS
    bool SetElementAsSignedShort( const DcmTag& tag, int value, unsigned long pos = 0 ); // type SS
    bool SetElementAsUnsignedShort( const DcmTag& tag, int value, unsigned long pos = 0 ); // type US


    /**
      \brief Get a human-readable version of patient position enumerations used e.g. in DICOM series.
    */
    static QString TranslateDefinedTermPatientPosition( const QString& dt );

    /**
      \brief Get a human-readable version of modality enumerations used e.g. in DICOM series.
    */
    static QString TranslateDefinedTermModality( const QString& dt );

    /**
      \brief Nicely formatted (group,element) version of a tag
    */
    static QString TagKey( const DcmTag& tag );

    /**
      \brief Description (name) of the tag
    */
    static QString TagDescription( const DcmTag& tag );

    /**
      \brief Value Representation
    */
    static QString TagVR( const DcmTag& tag );

protected:

    /**
      \brief Callback for retrieving a serialized version of this class

      You can override this method in a subclass to retrieve a serialized
      version of the object from some storage mechanism, eg a database
    */
    virtual QString GetStoredSerialization();

    /**
      \brief Callback for storing a serialized version of this class

      You can override this method in a subclass to store a serialized
      version of the object to some storage mechanism, eg a database
    */
    virtual void SetStoredSerialization(QString serializedDataset);

  QScopedPointer<ctkDICOMDatasetPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkDICOMDataset);
};

#endif

