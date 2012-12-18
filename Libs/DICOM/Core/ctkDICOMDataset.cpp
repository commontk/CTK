/*=============================================================================

  Library: CTK

  Copyright (c) German Cancer Research Center,
    Division of Medical and Biological Informatics

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "ctkDICOMDataset.h"

#include <dctk.h>
#include <dcostrmb.h>
#include <dcistrmb.h>

#include <stdexcept>


class ctkDICOMDatasetPrivate
{
  public:

    ctkDICOMDatasetPrivate() : m_DcmDataset(0), m_TakeOwnership(true) {}

    QString m_SpecificCharacterSet;

    bool m_DICOMDataSetInitialized;
    bool m_StrictErrorHandling;

    DcmDataset* m_DcmDataset;
    bool m_TakeOwnership;
};


ctkDICOMDataset::ctkDICOMDataset(bool strictErrorHandling)
:d_ptr(new ctkDICOMDatasetPrivate)
{
  Q_D(ctkDICOMDataset);
  d->m_DcmDataset = new DcmDataset();
  d->m_TakeOwnership = true;
  d->m_DICOMDataSetInitialized = false;
  d->m_StrictErrorHandling = strictErrorHandling;
}

ctkDICOMDataset::~ctkDICOMDataset()
{
  Q_D(ctkDICOMDataset);
  if ( d->m_TakeOwnership)
  {
    delete d->m_DcmDataset;
  }
}


void ctkDICOMDataset::InitializeFromDataset(DcmDataset* dataset, bool takeOwnership)
{
  Q_D(ctkDICOMDataset);

  if(d->m_DcmDataset != dataset)
  {
    if (d->m_TakeOwnership)
    {
      delete d->m_DcmDataset;
    }
    d->m_DcmDataset = NULL;
  }

  if (dataset)
  {
    d->m_DcmDataset=dataset;
    d->m_TakeOwnership = takeOwnership;
    if (!d->m_DICOMDataSetInitialized)
    {
      d->m_DICOMDataSetInitialized = true;
      OFString encoding;
      if ( CheckCondition( dataset->findAndGetOFString(DCM_SpecificCharacterSet, encoding) ) )
      {
        d->m_SpecificCharacterSet = encoding.c_str();
      }
      }
      if (d->m_SpecificCharacterSet.isEmpty())
      {
        ///
        /// see Bug # 6458:
        /// There are cases, where different studies of the same person get encoded both with and without the SpecificCharacterSet attribute set.
        /// DICOM says: default is ASCII / ISO_IR 6 / ISO 646
        /// Since we experienced such mixed data, we supplement missing characterset information with the ISO_IR 100 / Latin1 character set.
        /// Since Latin1 is a superset of ASCII, this will not cause problems. PLUS in most cases (Europe) we will guess right and suppress
        /// "double patients" in applications.
        ///
        SetElementAsString( DCM_SpecificCharacterSet, "ISO_IR 100" );
      }
    }
  }


void ctkDICOMDataset::InitializeFromFile(const QString& filename,
                                         const E_TransferSyntax readXfer,
                                         const E_GrpLenEncoding groupLength,
                                         const Uint32 maxReadLength,
                                         const E_FileReadMode readMode)
{
  DcmDataset *dataset;

  DcmFileFormat fileformat;
  OFCondition status = fileformat.loadFile(filename.toAscii().data(), readXfer, groupLength, maxReadLength, readMode);
  dataset = fileformat.getAndRemoveDataset();

  if (!status.good())
  {
    qDebug() << "Could not load " << filename << "\nDCMTK says: " << status.text();
    delete dataset;
    return;
  }

  InitializeFromDataset(dataset, true);
}

void ctkDICOMDataset::Serialize()
{
  Q_D(ctkDICOMDataset);
  EnsureDcmDataSetIsInitialized();

  // store content of current DcmDataset (our parent) as QByteArray into m_ctkDICOMDataset
  Uint32 buffersize = 1024*1024; // reserve 1MB
  char* writebuffer = new char[buffersize];

  // write into buffer
  DcmOutputBufferStream dcmbuffer(writebuffer, buffersize);
  d->m_DcmDataset->transferInit();
  OFCondition condition = d->m_DcmDataset->write(dcmbuffer, EXS_LittleEndianImplicit, EET_UndefinedLength, NULL );
  d->m_DcmDataset->transferEnd();
  if ( condition.bad() )
  {
    std::cerr << "Could not DcmDataset::write(..): " << condition.text() << std::endl;
  }

  // get written contents of buffer
  offile_off_t datasetsize = 0;
  void* readbuffer = NULL;
  dcmbuffer.flushBuffer(readbuffer, datasetsize);

  //std::cerr << "** " << (void*)this << " ctkDICOMDataset: Serializing Dataset into " << datasetsize << " bytes" << std::endl;

  // construct Qt type from that contents
  QByteArray qtArray = QByteArray::fromRawData( static_cast<const char*>(readbuffer), datasetsize );
  //std::cerr << "** Buffer size: " << qtArray.size() << std::endl;
  QString stringbuffer = QString::fromAscii(qtArray.toBase64());

  //std::cerr << "** String of size " << stringbuffer.size() << " looks like this:\n" << stringbuffer.toStdString() << std::endl << std::endl;

  this->SetStoredSerialization( stringbuffer );

  delete[] writebuffer;
}

void ctkDICOMDataset::MarkForInitialization()
{
  Q_D(ctkDICOMDataset);
  d->m_DICOMDataSetInitialized = false;
}
bool ctkDICOMDataset::IsInitialized() const
{
  Q_D(const ctkDICOMDataset);
  return d->m_DICOMDataSetInitialized;
}
void ctkDICOMDataset::EnsureDcmDataSetIsInitialized() const
{
  if ( ! this->IsInitialized() )
  {
      throw std::logic_error("Calling methods on uninitialized ctkDICOMDataset");
  }
  // const_cast<ctkDICOMDataset*>(this)->Deserialize();
}

void ctkDICOMDataset::Deserialize()
{
  Q_D(ctkDICOMDataset);
  // read attribute m_ctkDICOMDataset
  // construct a DcmDataset from it
  // calls InitializeData(DcmDataset*)

  // this method can be called both from sub-classes when they get the InitializeData signal from the persistence framework
  // and from EnsureDcmDataSetIsInitialized() when a GetElement.. or SetElement.. method is called.

  if (d->m_DICOMDataSetInitialized) return; // only need to do this once

  QString stringbuffer = this->GetStoredSerialization();
  if ( stringbuffer.isEmpty() )
  {
    d->m_DICOMDataSetInitialized = true;
    return; // TODO nicer: hold three states: newly created / loaded but not initialized / restored from DB
  }


  //std::cerr << "** " << (void*)this << " ctkDICOMDataset: Deserialize Dataset from string of size " << stringbuffer.size() << "\n" << stringbuffer.toStdString() << std::endl;

  QByteArray qtArray = QByteArray::fromBase64( stringbuffer.toAscii() );
  //std::cerr << "** " << (void*)this << " ctkDICOMDataset: Deserialize Dataset from byte array of size " << qtArray.size() << std::endl;

  DcmInputBufferStream dcmbuffer;
  dcmbuffer.setBuffer( qtArray.data(), qtArray.size() );
  //std::cerr << "** Buffer state: " << dcmbuffer.status().code() << " " <<  dcmbuffer.good() << " " << dcmbuffer.eos() << " tell " << dcmbuffer.tell() << " avail " << dcmbuffer.avail() << std::endl;

  DcmDataset dataset;
  dataset.transferInit();
  //std::cerr << "** Dataset state: " << dataset.transferState() << std::endl << std::endl;
  OFCondition condition = dataset.read( dcmbuffer, EXS_LittleEndianImplicit );
  dataset.transferEnd();

  // do this in all cases, even when reading reported an error
  this->InitializeFromDataset(&dataset);

  if ( condition.bad() )
  {
    std::cerr << "** Condition code of Dataset::read() is "
              << condition.code() << std::endl;
    std::cerr << "** Buffer state: " << dcmbuffer.status().code()
              << " " <<  dcmbuffer.good()
              << " " << dcmbuffer.eos()
              << " tell " << dcmbuffer.tell()
              << " avail " << dcmbuffer.avail() << std::endl;
    std::cerr << "** Dataset state: "
              << static_cast<int>(dataset.transferState()) << std::endl;
    std::cerr << "Could not DcmDataset::read(..): "
              << condition.text() << std::endl;
    //throw std::invalid_argument( std::string("Could not DcmDataset::read(..): ") + condition.text() );
  }
}

DcmDataset& ctkDICOMDataset::GetDcmDataset() const
{
  const Q_D(ctkDICOMDataset);
  return *d->m_DcmDataset;
}

OFCondition ctkDICOMDataset::findAndGetElement(const DcmTag& tag, DcmElement*& element, const OFBool searchIntoSub) const
{
  EnsureDcmDataSetIsInitialized();
  // this one const_cast allows us to declare quite a lot of methods nicely with const
  return GetDcmDataset().findAndGetElement(tag, element, searchIntoSub);
}

OFCondition ctkDICOMDataset::findAndGetOFString(const DcmTag& tag, OFString& value, const unsigned long pos, const OFBool searchIntoSub) const
{
  EnsureDcmDataSetIsInitialized();
  // this second const_cast allows us to declare quite a lot of methods nicely with const
  return GetDcmDataset().findAndGetOFString(tag, value, pos, searchIntoSub);
}

bool ctkDICOMDataset::CheckCondition(const OFCondition& condition)
{
  if ( condition.bad() )
  {
    //std::cerr << "Bad return code (" << condition.code() << "): " << condition.text() << std::endl;
  }

  return condition.good();
}

bool ctkDICOMDataset::CopyElement( DcmDataset* dataset, const DcmTagKey& tag, int type )
{
  Q_D(ctkDICOMDataset);
  switch (type)
  {
    case 0x1:
    case 0x1C:
    case 0x2:
    case 0x2C:
    case 0x3:
      // these are ok
      break;
    default:
      // nothing else is ok
      std::cerr << "Unknown attribute type. Cannot process call to ExtractElement " << TagKey(tag).toStdString() << std::endl;
      return false;
  }

  bool missing(false);
  bool copied(true);

  if (!dataset) return false;
  if (dataset == d->m_DcmDataset)
  {
    throw std::logic_error("Trying to copy tag to yourself. Please check application logic!"); 
  }

  // type 1 or 1C must exist AND have a value
  if (!dataset->tagExistsWithValue( tag ))
  {
    if (type == 0x1 || type == 0x1C) missing = true;
  }

  // type 2 or 2C must exist but may have an empty value
  if (!dataset->tagExists( tag ))
  {
    if (type == 0x1 || type == 0x1C) missing = true;
    if (type == 0x2 || type == 0x2C) missing = true;
  }
  else
  {
    // we found this tag
    DcmElement* element(NULL);
    dataset->findAndGetElement( tag, element, OFFalse, OFTrue ); // OFTrue is important (copies element), DcmDataset takes ownership and deletes elements on its own destruction
    if (element)
    {
      copied = CheckCondition( d->m_DcmDataset->insert(element) );
    }
  }

  if (missing)
  {
    std::cerr << "Tag " << TagKey(tag).toStdString() << " [" << TagDescription(tag).toStdString() << "] of type " << QString("%1").arg(type,0,16).toStdString() << " missing or empty." << std::endl;
  }

  if (!copied)
  {
    std::cerr << "Tag " << TagKey(tag).toStdString() << " [" << TagDescription(tag).toStdString() << "] not copied successfully" << std::endl;
  }

  return !missing && copied;
}

QString ctkDICOMDataset::Decode( const DcmTag& tag, const OFString& raw ) const
{
  Q_D(const ctkDICOMDataset);
  // decode for types LO, LT, PN, SH, ST, UT
  QString vr = TagVR(tag);
  if ( !d->m_SpecificCharacterSet.isEmpty()
    && (vr == "LO" ||
        vr == "LT" ||
        vr == "PN" ||
        vr == "SH" ||
        vr == "ST" ||
        vr == "UT" ) )
  {
    //std::cout << "Decode from encoding " << d->m_SpecificCharacterSet.toStdString() << std::endl;
    static QMap<QString, QTextDecoder*> decoders;
    static QMap<QString, QString> qtEncodingNamesForDICOMEncodingNames;

    if (qtEncodingNamesForDICOMEncodingNames.isEmpty())
    {
      // runs only once and fills up a map of encoding names that might be named in DICOM files.
      // for each encoding we store the name that Qt uses for the same encoding.
      // This is because there is not yet a standard naming scheme but lots of aliases
      // out in the real world: e.g. http://www.openi18n.org/subgroups/sa/locnameguide/final/CodesetAliasTable.html

                                              //    DICOM        Qt
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 6", "UTF-8"); // actually ASCII, but ok
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 100", "ISO-8859-1");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 101", "ISO-8859-2");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 109", "ISO-8859-3");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 110", "ISO-8859-4");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 144", "ISO-8859-5");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 127", "ISO-8859-6");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 126", "ISO-8859-7");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 138", "ISO-8859-8");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 148", "ISO-8859-9");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 179", "ISO-8859-13");
      qtEncodingNamesForDICOMEncodingNames.insert("ISO_IR 192", "UTF-8");
      // japanese
      qtEncodingNamesForDICOMEncodingNames.insert("ISO 2022 IR 13", "ISO 2022-JP"); // Single byte charset, JIS X 0201: Katakana, Romaji
      qtEncodingNamesForDICOMEncodingNames.insert("ISO 2022 IR 87", "ISO 2022-JP"); // Multi byte charset, JIS X 0208: Kanji, Kanji set
      qtEncodingNamesForDICOMEncodingNames.insert("ISO 2022 IR 159", "ISO 2022-JP");
      // korean
      qtEncodingNamesForDICOMEncodingNames.insert("ISO 2022 IR 149", "EUC-KR"); // Multi byte charset, KS X 1001: Hangul, Hanja

      // use all names that Qt knows by itself
      foreach( QByteArray c, QTextCodec::availableCodecs() )
      {
        qtEncodingNamesForDICOMEncodingNames.insert( c.constData(), c.constData() );
      }

    }

    if ( qtEncodingNamesForDICOMEncodingNames.contains(d->m_SpecificCharacterSet) )
    {
      QString encodingName( qtEncodingNamesForDICOMEncodingNames[d->m_SpecificCharacterSet] );
      if ( !decoders.contains( encodingName ) )
      {
        QTextCodec* codec = QTextCodec::codecForName( encodingName.toAscii() );
        if (!codec)
        {
          std::cerr << "Could not create QTextCodec object for '" << encodingName.toStdString() << "'. Using default encoding instead." << std::endl;
          decoders.insert( encodingName, QTextCodec::codecForCStrings()->makeDecoder() ); // uses Latin1
        }
        else
        {
          // initialize a QTextDecoder for given encoding
          decoders.insert( encodingName, codec->makeDecoder() );
          // We are responsible for deleting the QTextDecoder objects
          // created by makeDecoder(). BUT as these objects are stored
          // in a static map that lives until application end AND
          // nothing application relevant happens during their
          // destruction, we just let them be destructed by C++ on
          // application exit.
          // Any potential leaks that are found by this behavior can
          // be suppressed.
        }
      }

      //std::cout << "Decode '" <<  raw.c_str() << "' to '" << decoders[encodingName]->toUnicode( raw.c_str() ).toLocal8Bit().constData() << "'" << std::endl;
      return decoders[encodingName]->toUnicode( raw.c_str() );
    }
    else
    {
      std::cerr << "DICOM dataset contains some encoding that we never thought we would see(" << d->m_SpecificCharacterSet.toStdString() << "). Using default encoding." << std::endl;
    }
  }

  return QString::fromLatin1(raw.c_str()); // Latin1 is ISO 8859, which is the default character set of DICOM (PS 3.5-2008, Page 18)

}

OFString ctkDICOMDataset::Encode( const DcmTag& tag, const QString& qstring ) const
{
  // TODO: respect given character-set when encoding; see Decode()
  Q_UNUSED(tag);
  return OFString( qstring.toLatin1().data() ); // Latin1 is ISO 8859, which is the default character set of DICOM (PS 3.5-2008, Page 18)
}

QString ctkDICOMDataset::GetAllElementValuesAsString( const DcmTag& tag ) const
{
  this->EnsureDcmDataSetIsInitialized();

  QStringList qsl;

  DcmElement* element(NULL);
  findAndGetElement(tag, element);
  if (!element) return QString::null;

  const unsigned long count = element->getVM(); // value multiplicity
  for (unsigned long i = 0; i < count; ++i)
  {
    OFString s;
    if ( CheckCondition( const_cast<ctkDICOMDataset*>(this)->findAndGetOFString(tag, s, i) ) )
    {
      qsl << Decode( tag, s );
    }
  }

  return qsl.join("\\");
}


QString ctkDICOMDataset::GetElementAsString( const DcmTag& tag, unsigned long pos ) const
{
  this->EnsureDcmDataSetIsInitialized();

  OFString s;
  if ( CheckCondition( findAndGetOFString(tag, s, pos) ) )
  {
    return Decode( tag, s );
  }
  else
  {
    return QString::null;
  }
}

QStringList ctkDICOMDataset::GetElementAsStringList( const DcmTag& tag ) const
{
  this->EnsureDcmDataSetIsInitialized();
  QStringList qsl;

  DcmElement* element(NULL);
  findAndGetElement(tag, element);
  if (!element) return qsl;

  const unsigned long count = element->getVM(); // value multiplicity
  for (unsigned long i = 0; i < count; ++i)
  {
    qsl << GetElementAsString(tag, i);
  }

  return qsl;
}

ctkDICOMPersonName ctkDICOMDataset::GetElementAsPersonName( const DcmTag& tag, unsigned long pos ) const
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmPersonName* name = dynamic_cast<DcmPersonName*>(element);

  if (!name) return ctkDICOMPersonName(); // invalid

  OFString lastName;
  OFString firstName;
  OFString middleName;
  OFString namePrefix;
  OFString nameSuffix;
  if (CheckCondition( name->getNameComponents(lastName, firstName, middleName, namePrefix, nameSuffix, pos) ) )
  {
    return ctkDICOMPersonName(
      Decode(tag, lastName),
      Decode(tag, firstName),
      Decode(tag, middleName),
      Decode(tag, namePrefix),
      Decode(tag, nameSuffix) );
  }
  else
  {
    return ctkDICOMPersonName();
  }
}

ctkDICOMPersonNameList ctkDICOMDataset::GetElementAsPersonNameList( const DcmTag& tag ) const
{
  this->EnsureDcmDataSetIsInitialized();
  ctkDICOMPersonNameList qpnl;

  DcmElement* element(NULL);
  findAndGetElement(tag, element);
  if (!element) return qpnl;

  const unsigned long count = element->getVM(); // value multiplicity
  for (unsigned long i = 0; i < count; ++i)
  {
    qpnl << GetElementAsPersonName(tag, i);
  }

  return qpnl;
}

QDate ctkDICOMDataset::GetElementAsDate( const DcmTag& tag, unsigned long pos ) const
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmDate* date = dynamic_cast<DcmDate*>(element);

  if (!date) return QDate(); // invalid

  OFString ofs;
  if (CheckCondition( date->getISOFormattedDate(ofs, pos) ) )
  {
    QString qs(ofs.c_str());
    return QDate::fromString(qs, "yyyy-MM-dd");
  }
  else
  {
    return QDate();
  }
}

QTime ctkDICOMDataset::GetElementAsTime( const DcmTag& tag, unsigned long pos ) const
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmTime* time = dynamic_cast<DcmTime*>(element);

  if (!time) return QTime(); // invalid

  OFString ofs;
  if (CheckCondition( time->getISOFormattedTime(ofs, pos, OFTrue, OFFalse) ) ) // true (seconds), false (fraction of a second)
  {
    QString qs(ofs.c_str());
    return QTime::fromString(qs, "hh:mm:ss");
  }
  else
  {
    return QTime();
  }
}

QDateTime ctkDICOMDataset::GetElementAsDateTime( const DcmTag& tag, unsigned long pos ) const
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmDateTime* datetime = dynamic_cast<DcmDateTime*>(element);

  if (!datetime) return QDateTime(); // invalid

  OFString ofs;
  if (CheckCondition( datetime->getISOFormattedDateTime(ofs, pos, OFTrue, OFFalse, OFTrue) ) ) // true (seconds), false (fraction of a second), true (time zone)
  {
    QString qs(ofs.c_str());
    return QDateTime::fromString(qs, "dd-MM-yyy hh:mm:ss");
  }
  else
  {
    return QDateTime();
  }
}

double ctkDICOMDataset::GetElementAsDouble( const DcmTag& tag, unsigned long pos ) const
{
  Q_D(const ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmDecimalString* ds = dynamic_cast<DcmDecimalString*>(element);

  if (!ds)
  {
    if (d->m_StrictErrorHandling)
    {
      throw std::logic_error("Element not found or not a decimal number");
    }
    else
    {
      return 0.0;
    }
  }
  Float64 dvalue;
  ds->getFloat64(dvalue, pos);

  return dvalue;
}

long ctkDICOMDataset::GetElementAsInteger( const DcmTag& tag, unsigned long pos ) const
{
  Q_D(const ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmIntegerString* is = dynamic_cast<DcmIntegerString*>(element);

  if (!is)
  {
    if (d->m_StrictErrorHandling)
    {
      throw std::logic_error("Element not found or not an integer");
    }
    else
    {
      return 0;
    }
  }

  Sint32 i = 0;
  is->getSint32(i, pos);

  return i;
}

int ctkDICOMDataset::GetElementAsSignedShort( const DcmTag& tag, unsigned long pos ) const // type SS
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmSignedShort* ss = dynamic_cast<DcmSignedShort*>(element);

  if (!ss) throw std::logic_error("Element not found or not a signed short integer");

  Sint16 i;
  ss->getSint16(i, pos);

  return i;
}

int ctkDICOMDataset::GetElementAsUnsignedShort( const DcmTag& tag, unsigned long pos ) const // type US
{
  this->EnsureDcmDataSetIsInitialized();
  DcmElement* element(NULL);
  findAndGetElement(tag, element);

  DcmUnsignedShort* us = dynamic_cast<DcmUnsignedShort*>(element);

  if (!us) throw std::logic_error("Element not found or not a unsigned short integer");

  Uint16 i;
  us->getUint16(i, pos);

  return i;
}

bool ctkDICOMDataset::SetElementAsString( const DcmTag& tag, QString string )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  // TODO: Evaluate DICOM tag for proper encoding (see GetElementAsString())
  return CheckCondition( d->m_DcmDataset->putAndInsertString( tag, string.toLatin1().data() ) );
}

bool ctkDICOMDataset::SetElementAsStringList( const DcmTag& /*tag*/, QStringList /*stringList*/ )
{
  this->EnsureDcmDataSetIsInitialized();
  // TODO: Find out how this can be implemented with DcmDataset methods; there is no method for
  // setting a string at a given position
  return false;
}

bool ctkDICOMDataset::SetElementAsPersonName( const DcmTag& tag, ctkDICOMPersonName personName )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  DcmPersonName* dcmPersonName = new DcmPersonName( tag ); // TODO leak?

  if ( CheckCondition( dcmPersonName->putNameComponents(
    Encode( tag, personName.lastName() ),
    Encode( tag, personName.firstName() ),
    Encode( tag, personName.middleName() ),
    Encode( tag, personName.namePrefix() ),
    Encode( tag, personName.nameSuffix() ) ) ) )
  {
    return CheckCondition( d->m_DcmDataset->insert( dcmPersonName ) );
  }

  return false;
}

bool ctkDICOMDataset::SetElementAsPersonNameList( const DcmTag& tag, ctkDICOMPersonNameList personNameList )
{
  Q_UNUSED(tag);
  Q_UNUSED(personNameList);
  this->EnsureDcmDataSetIsInitialized();
  // TODO: Find out how this can be implemented with DcmDataset methods; there is no method for
  // setting an element at a given position
  return false;
}

bool ctkDICOMDataset::SetElementAsDate( const DcmTag& tag, QDate date )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  OFDate ofDate( date.year(), date.month(), date.day() );
  DcmDate* dcmDate = new DcmDate( tag ); // TODO leak?

  if ( CheckCondition( dcmDate->setOFDate( ofDate ) ) )
  {
    return CheckCondition( d->m_DcmDataset->insert( dcmDate ) );
  }

  return false;
}

bool ctkDICOMDataset::SetElementAsTime( const DcmTag& tag, QTime time )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  OFTime ofTime( time.hour(), time.minute(), time.second() );
  DcmTime* dcmTime = new DcmTime( tag ); // TODO leak?

  if ( CheckCondition( dcmTime->setOFTime( ofTime ) ) )
  {
    return CheckCondition( d->m_DcmDataset->insert( dcmTime ) );
  }

  return false;
}

bool ctkDICOMDataset::SetElementAsDateTime( const DcmTag& tag, QDateTime dateTime )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  QDate date = dateTime.date();
  QTime time = dateTime.time();

  OFDateTime ofDateTime;
  ofDateTime.setDateTime( date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second() );
  DcmDateTime* dcmDateTime = new DcmDateTime( tag ); // TODO leak?

  if ( CheckCondition( dcmDateTime->setOFDateTime( ofDateTime ) ) )
  {
    return CheckCondition( d->m_DcmDataset->insert( dcmDateTime ) );
  }

  return false;
}

bool ctkDICOMDataset::SetElementAsInteger( const DcmTag& tag, long value, unsigned long pos )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  //std::cerr << "TagVR: " << TagVR( tag ).toStdString() << std::endl;
  return CheckCondition( d->m_DcmDataset->putAndInsertSint32( tag, value, pos ) );
}

bool ctkDICOMDataset::SetElementAsSignedShort( const DcmTag& tag, int value, unsigned long pos )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  //std::cerr << "TagVR: " << TagVR( tag ).toStdString() << std::endl;
  return CheckCondition( d->m_DcmDataset->putAndInsertSint16( tag, value, pos ) );
}

bool ctkDICOMDataset::SetElementAsUnsignedShort( const DcmTag& tag, int value, unsigned long pos )
{
  Q_D(ctkDICOMDataset);
  this->EnsureDcmDataSetIsInitialized();
  //std::cerr << "TagVR: " << TagVR( tag ).toStdString() << std::endl;
  return CheckCondition( d->m_DcmDataset->putAndInsertUint16( tag, value, pos ) );
}

QString ctkDICOMDataset::GetStudyInstanceUID() const
{
  return this->GetElementAsString(DCM_StudyInstanceUID);
}

QString ctkDICOMDataset::GetSeriesInstanceUID() const
{
  return this->GetElementAsString(DCM_SeriesInstanceUID);
}

QString ctkDICOMDataset::GetSOPInstanceUID() const
{
  return this->GetElementAsString(DCM_SOPInstanceUID);
}

QString ctkDICOMDataset::TranslateDefinedTermPatientPosition( const QString& dt )
{
  static bool initialized = false;
  static QMap<QString, QString> descriptionOfTerms;
  if (!initialized)
  {
    descriptionOfTerms.insert("HFP",  "Head First - Prone");
    descriptionOfTerms.insert("HFDR", "Head First - Decubitus Right");
    descriptionOfTerms.insert("FFDR", "Feet First - Decubitus Right");
    descriptionOfTerms.insert("FFP",  "Feet First - Prone");
    descriptionOfTerms.insert("HFS",  "Head First - Supine");
    descriptionOfTerms.insert("HFDL", "Head First - Decubitus Left");
    descriptionOfTerms.insert("FFDL", "Feet First - Decubitus Left");
    descriptionOfTerms.insert("FFS",  "Feet First - Supine");
    initialized = true;
  }

  if ( descriptionOfTerms.contains( dt.toUpper() ) )
  {
    return descriptionOfTerms.value(dt.toUpper());
  }
  else
  {
    std::cerr << "Invalid enum for patient position" << std::endl;
    return QString::null;
  }
}

QString ctkDICOMDataset::TranslateDefinedTermModality( const QString& dt )
{
  static bool initialized = false;
  static QMap<QString, QString> descriptionOfTerms;
  if (!initialized)
  {
    descriptionOfTerms.insert("CR",  "Computed Radiography");
    descriptionOfTerms.insert("CT",  "Computed Tomography");
    descriptionOfTerms.insert("MR",  "Magnetic Resonance");
    descriptionOfTerms.insert("NM",  "Nuclear Medicine");
    descriptionOfTerms.insert("US",  "Ultrasound");
    descriptionOfTerms.insert("OT",  "Other");
    descriptionOfTerms.insert("BI",  "Biomagnetic imaging");
    descriptionOfTerms.insert("CD",  "Color flow Doppler");
    descriptionOfTerms.insert("DD",  "Duplex Doppler");
    descriptionOfTerms.insert("ES",  "Endoscopy");
    descriptionOfTerms.insert("LS",  "Laser surface scan");
    descriptionOfTerms.insert("PT",  "Positron emission tomography (PET)");
    descriptionOfTerms.insert("RG",  "Radiographic imaging (conventional film/screen)");
    descriptionOfTerms.insert("ST",  "Single-photon emission computed tomograpy (SPECT)");
    descriptionOfTerms.insert("TG",  "Thermography");
    descriptionOfTerms.insert("XA",  "X-Ray Aniography");
    descriptionOfTerms.insert("RF",  "Radio Fluoroscopy");
    descriptionOfTerms.insert("RTIMAGE",  "Radiotherapy Image");
    descriptionOfTerms.insert("RTDOSE",  "Radiotherapy Dose");
    descriptionOfTerms.insert("RTSTRUCT",  "Radiotherapy Structure Set");
    descriptionOfTerms.insert("RTPLAN",  "Radiotherapy Plan");
    descriptionOfTerms.insert("RTRECORD",  "RT Treatment Record");
    descriptionOfTerms.insert("HC",  "Hard Copy");
    descriptionOfTerms.insert("DX",  "Digital Radiography");
    descriptionOfTerms.insert("MG",  "Mammography");
    descriptionOfTerms.insert("IO",  "Intra-oral Radiography");
    descriptionOfTerms.insert("PX",  "Panoramic X-Ray");
    descriptionOfTerms.insert("GM",  "General Microscopy");
    descriptionOfTerms.insert("SM",  "Slide Microscopy");
    descriptionOfTerms.insert("XC",  "External-camera Photography");
    descriptionOfTerms.insert("PR",  "Presentation state");
    descriptionOfTerms.insert("AU",  "Audio");
    descriptionOfTerms.insert("ECG",  "Electrocardiography");
    descriptionOfTerms.insert("EPS",  "Cardiac Electrophysiology");
    descriptionOfTerms.insert("HD",  "Hemodynamic Waveform");
    descriptionOfTerms.insert("SR",  "SR Document");
    descriptionOfTerms.insert("IVUS",  "Intravascular Ultrasound");
    descriptionOfTerms.insert("OP",  "Ophthalmic Photography");
    descriptionOfTerms.insert("SMR",  "Stereometric Relationship");
    descriptionOfTerms.insert("OCT",  "Optical Coherence Tomography (non-Ophthalmic)");
    descriptionOfTerms.insert("OPR",  "Ophthalmic Refraction");
    descriptionOfTerms.insert("OPV",  "Ophthalmic Visual Field");
    descriptionOfTerms.insert("OPM",  "Ophthalmic Mapping");
    descriptionOfTerms.insert("KO",  "Key Object Selection");
    descriptionOfTerms.insert("SEG",  "Segmentation");
    descriptionOfTerms.insert("REG",  "Registration");
    descriptionOfTerms.insert("OPT",  "Ophthalmic Tomography");
    descriptionOfTerms.insert("BDUS",  "Bone Densitometry (ultrasound)");
    descriptionOfTerms.insert("BMD",  "Bone Densitometry (X-Ray)");
    descriptionOfTerms.insert("DOC",  "Document");

    // retired terms (but probably still in use)
    descriptionOfTerms.insert("DS",  "Digital Subtraction Angiography");
    descriptionOfTerms.insert("CF",  "Cinefluorography");
    descriptionOfTerms.insert("DF",  "Digital fluoroscopy");
    descriptionOfTerms.insert("VF",  "Videofluorography");
    descriptionOfTerms.insert("AS",  "Angioscopy");
    descriptionOfTerms.insert("CS",  "Cystoscopy");
    descriptionOfTerms.insert("EC",  "Echocardiography");
    descriptionOfTerms.insert("LP",  "Laparoscopy");
    descriptionOfTerms.insert("FA",  "Fluorescein angiography ");
    descriptionOfTerms.insert("CP",  "Culposcopy");
    descriptionOfTerms.insert("DM",  "Digital microscopy");
    descriptionOfTerms.insert("FS",  "Fundoscopy");
    descriptionOfTerms.insert("MA",  "Magnetic resonance angiography");
    descriptionOfTerms.insert("MS",  "Magnetic resonance spectroscopy");
    initialized = true;
  }

  if ( descriptionOfTerms.contains( dt.toUpper() ) )
  {
    return descriptionOfTerms.value(dt.toUpper());
  }
  else
  {
    std::cerr << "Invalid enum for patient position" << std::endl;
    return QString::null;
  }
}

QString ctkDICOMDataset::TagKey( const DcmTag& tag )
{
  return QString("(%1,%2)").arg( tag.getGroup(), 4, 16, QLatin1Char('0')).arg( tag.getElement(), 4, 16, QLatin1Char('0') );
}

QString ctkDICOMDataset::TagDescription( const DcmTag& tag )
{
  if (!dcmDataDict.isDictionaryLoaded())
    return QString("<no DICOM dictionary loaded. application broken>");
  const DcmDataDictionary& globalDict = dcmDataDict.rdlock();
  const DcmDictEntry* entry = globalDict.findEntry(tag, NULL);
  QString returnName("Unknown");
  if (entry)
  {
    returnName = entry->getTagName();
  }
  dcmDataDict.unlock();
  return returnName;
}

QString ctkDICOMDataset::TagVR( const DcmTag& tag )
{
  if (!dcmDataDict.isDictionaryLoaded()) return QString("<no DICOM dictionary loaded. application broken>");
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry* entry = globalDataDict.findEntry(tag, NULL);
  QString returnVR("UN");
  if (entry)
  {
    returnVR = entry->getVR().getVRName();
  }
  dcmDataDict.unlock();
  return returnVR;
}

QString ctkDICOMDataset::GetStoredSerialization()
{
  throw std::runtime_error("No serialization implemented for this object!");
}



void ctkDICOMDataset::SetStoredSerialization(QString serializedDataset)
{
  Q_UNUSED(serializedDataset);
  throw std::runtime_error("No serialization implemented for this object!");
}

bool ctkDICOMDataset::SaveToFile(const QString& filePath) const
{
  Q_D(const ctkDICOMDataset);
  DcmFileFormat* fileformat = new DcmFileFormat ( d->m_DcmDataset );
  OFCondition status = fileformat->saveFile ( qPrintable(QDir::toNativeSeparators( filePath)) );
  delete fileformat;
  return status.good();
}

