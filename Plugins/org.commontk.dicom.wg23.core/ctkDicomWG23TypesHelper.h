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
class ctkDicomSoapArrayOfString : public QtSoapArray{
    public:
    ctkDicomSoapArrayOfString ( const QString & name, const QStringList array):
            QtSoapArray ( QtSoapQName(name),QtSoapType::String,
                          array.size()){

        for (QStringList::ConstIterator it = array.constBegin();
                it < array.constEnd(); it++){
            this->append(new QtSoapSimpleType(QtSoapQName("string"),*it));
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

//Not easy to template, will see later for other types
class ctkDicomSoapArrayOfString : public QtSoapArray{
    public:
    ctkDicomSoapArrayOfString ( const QString & name, const QStringList array):
            QtSoapArray ( QtSoapQName(name),QtSoapType::String,
                          array.size()){

        for (QStringList::ConstIterator it = array.constBegin();
                it < array.constEnd(); it++){
            this->append(new QtSoapSimpleType(QtSoapQName("string"),*it));
        }
    };

    static QStringList* getArray(const QtSoapArray& array){
        QStringList * list = new QStringList();
        for (int i; i < array.count() ; i++ ){
            const QString str = array.at(i).value().toString();
            list->append( str);
        }
        return list;
    };

};


#endif
