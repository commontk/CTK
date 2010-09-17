/*=============================================================================

  Library: CTK

  Copyright (c) 2010 German Cancer Research Center,
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


#ifndef CTKDICOM23TYPESHELPER_H
#define CTKDICOM23TYPESHELPER_H

#include <QtSoapStruct>
#include <QtSoapQName>
#include <QRect>
#include <ctkDicomWG23Types.h>

#include <stdexcept>

class ctkDicomSoapRectangle :public QtSoapStruct {
    public: 
    ctkDicomSoapRectangle(const QString & name,const QRect& rect):
        QtSoapStruct(QtSoapQName(name)){
        this->insert(new QtSoapSimpleType(QtSoapQName("Height"), 
                    rect.height()));
        this->insert(new QtSoapSimpleType(QtSoapQName("Width"), 
                    rect.width()));
        this->insert(new QtSoapSimpleType(QtSoapQName("RefPointX"),
                    rect.x()));
        this->insert(new QtSoapSimpleType(QtSoapQName("RefPointY"), 
                    rect.y()));
    };

    static QRect getQRect (const QtSoapType& type){
        return QRect (type["RefPointX"].value().toInt(),
                        type["RefPointY"].value().toInt(),
                        type["Width"].value().toInt(),
                        type["Height"].value().toInt());
    };
};

class ctkDicomSoapState : public QtSoapSimpleType{
    public:
   ctkDicomSoapState ( const QString & name, ctkDicomWG23::State s ):
       QtSoapSimpleType ( QtSoapQName(name), toStringValue(s) ){};

   static ctkDicomWG23::State getState(const QtSoapType& type){
        return fromString( type.value().toString() );
   };

   static ctkDicomWG23::State fromString(QString string)
   {
     if (string == "IDLE") return ctkDicomWG23::IDLE;
     if (string == "INPROGRESS") return ctkDicomWG23::INPROGRESS;
     if (string == "COMPLETED") return ctkDicomWG23::COMPLETED;
     if (string == "SUSPENDED") return ctkDicomWG23::SUSPENDED;
     if (string == "CANCELED") return ctkDicomWG23::CANCELED;
     if (string == "EXIT") return ctkDicomWG23::EXIT;
     throw std::runtime_error( string.prepend("Invalid STATE:").toStdString() );
     return ctkDicomWG23::EXIT;
   }
   static QString toStringValue(ctkDicomWG23::State state)
   {
     switch(state)
     {
     case ctkDicomWG23::IDLE:
         return "IDLE";
     case ctkDicomWG23::INPROGRESS:
         return "INPROGRESS";
     case ctkDicomWG23::COMPLETED:
         return "COMPLETED";
     case ctkDicomWG23::SUSPENDED:
         return "SUSPENDED";
     case ctkDicomWG23::CANCELED:
         return "CANCELED";
     case ctkDicomWG23::EXIT:
         return "EXIT";
     default:
       throw std::runtime_error( "Invalid value for state" );

     }
   }

};


class ctkDicomSoapStatus : public QtSoapStruct{
    public: 
    ctkDicomSoapStatus ( const QString & name,
            const ctkDicomWG23::Status& s ):
       QtSoapStruct ( QtSoapQName(name) ){
        this->insert(new QtSoapSimpleType(QtSoapQName("StatusType"), 
                    s.statusType) );
        this->insert(new QtSoapSimpleType(
                    QtSoapQName("CodingSchemeDesignator"), 
                    s.codingSchemeDesignator) );
        this->insert(new QtSoapSimpleType(
                    QtSoapQName("CodeValue"), 
                    s.codeValue) );
        this->insert(new QtSoapSimpleType(
                    QtSoapQName("CodeMeaning"), 
                    s.codeMeaning) );
    };
   static ctkDicomWG23::Status getStatus(const QtSoapType& type){
        ctkDicomWG23::Status s;
        
        s.statusType = static_cast<ctkDicomWG23::StatusType>
            (type["StatusType"].value().toInt());
        s.codingSchemeDesignator = 
            type["CodingSchemeDesignator"].value().toString();
        s.codeValue = 
            type["CodeValue"].value().toString();
        s.codeMeaning = 
            type["CodeMeaning"].value().toString();
        return s;
   };
};

class ctkDicomSoapUID : public QtSoapSimpleType{
    public:
    ctkDicomSoapUID ( const QString & name, const QString& uid ):
   QtSoapSimpleType ( QtSoapQName(name), uid ){};

    static QString getUID(const QtSoapType& type){
        return type.value().toString();
    };
};

class ctkDicomSoapBool : public QtSoapSimpleType{
    public:
    ctkDicomSoapBool ( const QString & name, const bool& boolean ):
   QtSoapSimpleType ( QtSoapQName(name), boolean ){};

    static bool getBool(const QtSoapType& type){
        return  type.value().toBool();
    };

};

//Not easy to template, will see later for other types
class ctkDicomSoapArrayOfStringType : public QtSoapArray{
    public:
    ctkDicomSoapArrayOfStringType ( const QString& typeName,
                                const QString & name, const QStringList array):
            QtSoapArray ( QtSoapQName(name),QtSoapType::String,
                          array.size()){

        for (QStringList::ConstIterator it = array.constBegin();
                it < array.constEnd(); it++){
            this->append(new QtSoapSimpleType(QtSoapQName(typeName),*it));
        }
    };

    static QStringList* getArray(const QtSoapArray& array){
        QStringList * list = new QStringList();
        for (int i = 0; i < array.count() ; i++ ){
            const QString str = array.at(i).value().toString();
            list->append( str);
        }
        return list;
    };

};

class ctkDicomSoapArrayOfUUIDS : public QtSoapArray{
    public:
    ctkDicomSoapArrayOfUUIDS ( const QString & name, const QList<QUuid> &array):
            QtSoapArray ( QtSoapQName(name),QtSoapType::String,
                          array.size()){

        for (QList<QUuid>::ConstIterator it = array.constBegin();
                it < array.constEnd(); it++){
            this->append(new QtSoapSimpleType(QtSoapQName("UUID"),(*it).toString()));
        }
    };

    static QList<QUuid>* getArray(const QtSoapArray& array){
        QList<QUuid> * list = new QList<QUuid>();
        for (int i = 0; i < array.count() ; i++ ){
            const QString str = array.at(i).value().toString();
            list->append( QUuid(str));
        }
        return list;
    };

};

class ctkDicomSoapObjectDescriptor : public QtSoapStruct{
public:
    ctkDicomSoapObjectDescriptor ( const QString & name,
        const ctkDicomWG23::ObjectDescriptor& od ):
    QtSoapStruct ( QtSoapQName(name) ){
        this->insert(new QtSoapSimpleType(QtSoapQName("DescriptorUUID"),
                od.descriptorUUID) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("MimeType"),
                od.mimeType) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("ClassUID"),
                od.classUID) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("TransferSyntaxUID"),
                od.transferSyntaxUID) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("Modality"),
                od.modality) );
    };

    static ctkDicomWG23::ObjectDescriptor getObjectDescriptor(const QtSoapType& type){
         ctkDicomWG23::ObjectDescriptor od;

         od.descriptorUUID = QUuid(type["DescriptorUUID"].value().toString());
         od.mimeType =
             type["MimeType"].value().toString();
         od.classUID =
             type["ClassUID"].value().toString();
         od.transferSyntaxUID =
             type["TransferSyntaxUID"].value().toString();
         od.modality =
             type["Modality"].value().toString();
         return od;
     };
};

class ctkDicomSoapSeries : public QtSoapStruct{
    public:
        ctkDicomSoapSeries ( const QString & name,
            const ctkDicomWG23::Series& s ):
        QtSoapStruct ( QtSoapQName(name) ){
            this->insert(new QtSoapSimpleType(QtSoapQName("SeriesUID"),
                    s.seriesUID) );
        QtSoapArray * odescriptors = new QtSoapArray( QtSoapQName("ObjectDescriptors"),QtSoapType::Other,
                          s.objectDescriptors.size());

        for (QList<ctkDicomWG23::ObjectDescriptor>::ConstIterator it = s.objectDescriptors.constBegin();
                it < s.objectDescriptors.constEnd(); it++){
            odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
        }
        this->insert(odescriptors);
    };

    static ctkDicomWG23::Series getSeries(const QtSoapType& type){
         ctkDicomWG23::Series s;

         s.seriesUID = type["SeriesUID"].value().toString();
         QList<ctkDicomWG23::ObjectDescriptor> list;
         const QtSoapArray& array = static_cast<const QtSoapArray&> (type["ObjectDescriptors"]);
         for (int i = 0; i < array.count() ; i++ ){
             const ctkDicomWG23::ObjectDescriptor od =
                     ctkDicomSoapObjectDescriptor::getObjectDescriptor(array.at(i));
             list.append(od);
         }
         s.objectDescriptors = list;
         return s;
     };
};



class ctkDicomSoapStudy : public QtSoapStruct{
    public:
        ctkDicomSoapStudy ( const QString & name,
            const ctkDicomWG23::Study& s ):
        QtSoapStruct ( QtSoapQName(name) ){
            this->insert(new QtSoapSimpleType(QtSoapQName("StudyUID"),
                    s.studyUID) );
        QtSoapArray * odescriptors = new QtSoapArray( QtSoapQName("ObjectDescriptors"),QtSoapType::Other,
                          s.objectDescriptors.size());

        for (QList<ctkDicomWG23::ObjectDescriptor>::ConstIterator it = s.objectDescriptors.constBegin();
                it < s.objectDescriptors.constEnd(); it++){
            odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
        }
        this->insert(odescriptors);

        QtSoapArray * series = new QtSoapArray( QtSoapQName("Series"),QtSoapType::Other,
                          s.series.size());

        for (QList<ctkDicomWG23::Series>::ConstIterator it = s.series.constBegin();
                it < s.series.constEnd(); it++){
            series->append(new ctkDicomSoapSeries("Series",*it));
        }
        this->insert(series);
    };

    static ctkDicomWG23::Study getStudy(const QtSoapType& type){
         ctkDicomWG23::Study s;

         s.studyUID = type["StudyUID"].value().toString();
         QList<ctkDicomWG23::ObjectDescriptor> list;
         const QtSoapArray& array = static_cast<const QtSoapArray&> (type["ObjectDescriptors"]);
         for (int i = 0; i < array.count() ; i++ ){
             const ctkDicomWG23::ObjectDescriptor od =
                     ctkDicomSoapObjectDescriptor::getObjectDescriptor(array.at(i));
             list.append(od);
         }
         s.objectDescriptors = list;
         QList<ctkDicomWG23::Series> listSeries;
         const QtSoapArray& array2 = static_cast<const QtSoapArray&> (type["Series"]);
         for (int i = 0; i < array2.count() ; i++ ){
             const ctkDicomWG23::Series series =
                     ctkDicomSoapSeries::getSeries(array2.at(i));
             listSeries.append(series);
         }
         s.series = listSeries;

         return s;
     };
};

class ctkDicomSoapPatient : public QtSoapStruct{
    public:
        ctkDicomSoapPatient ( const QString & name,
            const ctkDicomWG23::Patient& p ):
        QtSoapStruct ( QtSoapQName(name) ){
            this->insert(new QtSoapSimpleType(QtSoapQName("Name"),
                    p.name) );
            this->insert(new QtSoapSimpleType(QtSoapQName("ID"),
                    p.id) );
            this->insert(new QtSoapSimpleType(QtSoapQName("AssigningAuthority"),
                    p.assigningAuthority) );
            this->insert(new QtSoapSimpleType(QtSoapQName("Sex"),
                    p.sex) );
            this->insert(new QtSoapSimpleType(QtSoapQName("BirthDate"),
                    p.birthDate) );
            QtSoapArray * odescriptors = new QtSoapArray( QtSoapQName("ObjectDescriptors"),QtSoapType::Other,
                          p.objectDescriptors.size());

            for (QList<ctkDicomWG23::ObjectDescriptor>::ConstIterator it = p.objectDescriptors.constBegin();
                it < p.objectDescriptors.constEnd(); it++){
                odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
            }
            this->insert(odescriptors);

            QtSoapArray * study = new QtSoapArray( QtSoapQName("Studies"),QtSoapType::Other,
                              p.studies.size());

            for (QList<ctkDicomWG23::Study>::ConstIterator it = p.studies.constBegin();
                it < p.studies.constEnd(); it++){
                study->append(new ctkDicomSoapStudy("Study",*it));
            }
            this->insert(study);
        };

        static ctkDicomWG23::Patient getPatient(const QtSoapType& type){
             ctkDicomWG23::Patient p;

             p.name = type["Name"].value().toString();
             p.id = type["ID"].value().toString();
             p.assigningAuthority = type["AssigningAuthority"].value().toString();
             p.sex = type["Sex"].value().toString();
             p.birthDate = type["Birthdate"].value().toString();

             QList<ctkDicomWG23::ObjectDescriptor> list;
             const QtSoapArray& array = static_cast<const QtSoapArray&> (type["ObjectDescriptors"]);
             for (int i = 0; i < array.count() ; i++ ){
                 const ctkDicomWG23::ObjectDescriptor od =
                         ctkDicomSoapObjectDescriptor::getObjectDescriptor(array.at(i));
                 list.append(od);
             }

             p.objectDescriptors = list;
             QList<ctkDicomWG23::Study> listPatient;
             const QtSoapArray& array2 = static_cast<const QtSoapArray&> (type["Studies"]);
             for (int i = 0; i < array2.count() ; i++ ){
                 const ctkDicomWG23::Study study =
                         ctkDicomSoapStudy::getStudy(array2.at(i));
                 listPatient.append(study);
             }

             p.studies = listPatient;
             return p;
        };
};


class ctkDicomSoapAvailableData :public QtSoapStruct {
public:
    ctkDicomSoapAvailableData ( const QString & name,
            const ctkDicomWG23::AvailableData& ad ):
       QtSoapStruct ( QtSoapQName(name) ){
        QtSoapArray * odescriptors = new QtSoapArray( QtSoapQName("ObjectDescriptors"),QtSoapType::Other,
                      ad.objectDescriptors.size());

        for (QList<ctkDicomWG23::ObjectDescriptor>::ConstIterator it = ad.objectDescriptors.constBegin();
            it < ad.objectDescriptors.constEnd(); it++){
            odescriptors->append(new ctkDicomSoapObjectDescriptor("ObjectDescriptor",*it));
        }
        this->insert(odescriptors);

        QtSoapArray * patient = new QtSoapArray( QtSoapQName("Patient"),QtSoapType::Other,
                          ad.patients.size());

        for (QList<ctkDicomWG23::Patient>::ConstIterator it = ad.patients.constBegin();
            it < ad.patients.constEnd(); it++){
            patient->append(new ctkDicomSoapPatient("Patient",*it));
        }
        this->insert(patient);
    };
   static ctkDicomWG23::AvailableData getAvailableData (const QtSoapType& type){
        ctkDicomWG23::AvailableData ad;

        QList<ctkDicomWG23::ObjectDescriptor> list;
        const QtSoapArray& array = static_cast<const QtSoapArray&> (type["ObjectDescriptors"]);
        for (int i = 0; i < array.count() ; i++ ){
            const ctkDicomWG23::ObjectDescriptor od =
                    ctkDicomSoapObjectDescriptor::getObjectDescriptor(array.at(i));
            list.append(od);
        }
        ad.objectDescriptors = list;
        QList<ctkDicomWG23::Patient> listPatients;
        const QtSoapArray& array2 = static_cast<const QtSoapArray&> (type["Patients"]);
        for (int i = 0; i < array2.count() ; i++ ){
            const ctkDicomWG23::Patient patient =
                    ctkDicomSoapPatient::getPatient(array2.at(i));
            listPatients.append(patient);
        }
        ad.patients = listPatients;
        return ad;
   };
};

class ctkDicomSoapObjectLocator : public QtSoapStruct{
public:
    ctkDicomSoapObjectLocator ( const QString & name,
        const ctkDicomWG23::ObjectLocator& ol ):
    QtSoapStruct ( QtSoapQName(name) ){
        this->insert(new QtSoapSimpleType(QtSoapQName("Locator"),
                ol.locator) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("Source"),
                ol.source) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("TransfertSyntax"),
                ol.transferSyntax) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("Length"),
                ol.length) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("Offset"),
                ol.offset) );
        this->insert(new QtSoapSimpleType(
                QtSoapQName("URI"),
                ol.URI) );
    };

    static ctkDicomWG23::ObjectLocator getObjectLocator(const QtSoapType& type){
         ctkDicomWG23::ObjectLocator ol;

         ol.locator = QUuid(type["Locator"].value().toString());
         ol.source = QUuid(type["Source"].value().toString());
         ol.transferSyntax =
             type["TransferSyntax"].value().toString();
         ol.length =
             type["Length"].value().toInt();
         ol.offset =
             type["Offset"].value().toInt();
         ol.URI =
             type["URI"].value().toString();
         return ol;
     };
};


#endif
