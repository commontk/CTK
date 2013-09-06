/*=============================================================================

  Library: CTK

  Copyright (c) University of Sheffield

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

// Qt include
#include <QSharedData>
#include <QStandardItem>
#include <QString>
#include <QStringList>

// DCMTK includes
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"        /* for class OFStandard */

// CTK DICOM Core
#include "ctkDICOMObjectModel.h"

//------------------------------------------------------------------------------
class ctkDICOMObjectModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMObjectModel);
protected:
  ctkDICOMObjectModel* const q_ptr;

public:
  ctkDICOMObjectModelPrivate(ctkDICOMObjectModel&);
  virtual ~ctkDICOMObjectModelPrivate();
  void init();
  void traverseDataElements( DcmItem *dataset, QStandardItem *parent);
  QString getTagValue( DcmElement *dcmElem);
  QStandardItem* populateModelRow(const QString& tagName,const QString& tagValue, QStandardItem *parent);

  DcmFileFormat fileFormat;
  QStandardItem *rootItem;
};
//------------------------------------------------------------------------------
ctkDICOMObjectModelPrivate::ctkDICOMObjectModelPrivate(ctkDICOMObjectModel& o):q_ptr(&o)
{
}

//------------------------------------------------------------------------------
ctkDICOMObjectModelPrivate::~ctkDICOMObjectModelPrivate()
{
}
//------------------------------------------------------------------------------
void ctkDICOMObjectModelPrivate::init()
{
  Q_Q(ctkDICOMObjectModel);
  QStringList horizontalHeaderLabels;
  horizontalHeaderLabels.append( QString("Tag"));
  horizontalHeaderLabels.append( QString("Value"));
  q->setHorizontalHeaderLabels(horizontalHeaderLabels);
}
//------------------------------------------------------------------------------
void ctkDICOMObjectModelPrivate::traverseDataElements( DcmItem *dataset, QStandardItem *parent)
{
  DcmStack stack;
  dataset->nextObject( stack, OFTrue);
  for( ; stack.top(); dataset->nextObject( stack, OFFalse))
    {
    DcmObject *dO =  stack.top();
    // put in the visit node function
    QString tagValue = "";
    DcmTag tag = dO->getTag();
	  // std::cout<<tag;
    QString tagName = tag.getTagName();
    DcmTag tagKey = tag.getXTag();
	  // std::cout<< tagName.toUtf8().constData()<<std::endl;
    if( tagKey == DCM_SequenceDelimitationItem
    || tagKey == DCM_ItemDelimitationItem
    || "Item" == tagName)
      {
	    return;
	    }
  
    DcmElement *dcmElem = dynamic_cast<DcmElement *> (dO);
    tagValue = getTagValue(dcmElem);
  
    // Populate QStandardModel with current DICOM element tag name and value
    QStandardItem *tagItem = populateModelRow(tagName,tagValue,parent);
    
    // check if the DICOM object is a SQ Data element and extract the nested DICOM objects
    if( dcmElem && !dcmElem->isLeaf())
      {	 
      DcmSequenceOfItems* newNode = dynamic_cast<DcmSequenceOfItems*> (dcmElem);
      dO = newNode->nextInContainer( NULL);
      for( ; dO; dO = newNode->nextInContainer( dO))
        {
        DcmElement *dcmElem2 = dynamic_cast<DcmElement *> (dO);
        tag = dO->getTag();
        if( tag.getXTag() == DCM_Item)
          {
          traverseDataElements( dynamic_cast<DcmItem*> (dO),parent);
          } 
         }
        }
    }
}

//------------------------------------------------------------------------------
QString ctkDICOMObjectModelPrivate::getTagValue( DcmElement *dcmElem)
{
  QString tagValue = "";
  std::ostringstream value;
  OFString part;
  std::string sep;
  int mult = dcmElem->getVM();
  int pos;
  
  if( mult>1)
    {
    value << "[" << mult << "] ";
    }
 
  // TODO define max elem per line
  
  for( pos=0; pos < mult; pos++)
    {
    value << sep;
    OFCondition status = dcmElem->getOFString( part, pos);
    if( status.good())
		  {
		  value << part.c_str();
		  sep = ", ";
		  }
    }
    if( pos < mult-1)
      {
       value << " ...";
      }
  tagValue = value.str().c_str();
  
  return tagValue;
}

//------------------------------------------------------------------------------
 QStandardItem* ctkDICOMObjectModelPrivate::populateModelRow(const QString& tagName,const QString& tagValue, QStandardItem *parent)
 {
  // Create items
  QStandardItem *tagItem = new QStandardItem( tagName);
  QStandardItem *valItem = new QStandardItem( tagValue);
  // Insert items
  QList<QStandardItem *> modelRow;
  modelRow.append( tagItem);
  modelRow.append( valItem);
  parent->appendRow( modelRow);
  return tagItem;
 }

//------------------------------------------------------------------------------
ctkDICOMObjectModel::ctkDICOMObjectModel(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkDICOMObjectModelPrivate(*this))
{
  Q_D(ctkDICOMObjectModel);
  d->init();
 }

//------------------------------------------------------------------------------
ctkDICOMObjectModel::ctkDICOMObjectModel(const ctkDICOMObjectModel& other)
{
}

//------------------------------------------------------------------------------
ctkDICOMObjectModel::~ctkDICOMObjectModel()
{
}

//------------------------------------------------------------------------------
void ctkDICOMObjectModel::setFile(const QString &fileName)
{
  Q_D(ctkDICOMObjectModel);
 
 OFCondition status = d->fileFormat.loadFile( fileName.toLatin1().data());
  if( !status.good())
    {
		// TODO: Add through error
    }
  
  DcmDataset *dataset = d->fileFormat.getDataset();
  d->rootItem = ctkDICOMObjectModel::invisibleRootItem();
  d->traverseDataElements( dataset, d->rootItem);
}
