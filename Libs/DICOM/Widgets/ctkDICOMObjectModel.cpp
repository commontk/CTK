/*=============================================================================

  Library: CTK

  Copyright (c) Brigham and Women's Hospital (BWH).
  Copyright (c) University of Sheffield.

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
#include "dcmtk/ofstd/ofstd.h"

// CTK DICOM Core
#include "ctkDICOMObjectModel.h"
#include "ctkDICOMItem.h"

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
  void itemInsert(ctkDICOMItem* dicomItem, DcmItem *dataset, QStandardItem *parent);
  void seqInsert(ctkDICOMItem* dicomItem, DcmSequenceOfItems *dataset, QStandardItem *parent);
  QString getTagValue(ctkDICOMItem* dicomItem, DcmElement *dcmElem);
  QStandardItem* populateModelRow(const QString& tagName,const QString& tagHexName,
  const QString& tagValue, const QString& VRName,
  const QString& elementLengthQString, QStandardItem *parent);

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
  horizontalHeaderLabels.append( QString("Attribute"));
  horizontalHeaderLabels.append( QString("Value"));
  horizontalHeaderLabels.append( QString("VR"));
  horizontalHeaderLabels.append( QString("Length"));
  q->setHorizontalHeaderLabels(horizontalHeaderLabels);
}

//------------------------------------------------------------------------------
void ctkDICOMObjectModelPrivate::itemInsert(ctkDICOMItem* dicomItem, DcmItem *dataset, QStandardItem *parent)
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
    //
    DcmTagKey tagX = tag.getXTag();
    QString tagHexName = tagX.toString().c_str();
    //
    DcmVR VR = dO->getVR();
    QString VRName = VR.getVRName();
    //
    // Getting length
    int elementLength;
    elementLength = dO->getLength();
    QString elementLengthQString = QString::number(elementLength);
    //
    DcmTag tagKey = tag.getXTag();
    if( tagKey == DCM_SequenceDelimitationItem
        || tagKey == DCM_ItemDelimitationItem
        || "Item" == tagName)
      {
      return;
      }

    DcmElement *dcmElem = dynamic_cast<DcmElement *> (dO);
    tagValue = getTagValue(dicomItem, dcmElem);

    // Populate QStandardModel with current DICOM element tag name and value
    QStandardItem *tagItem = populateModelRow(tagName,tagHexName,tagValue,VRName,elementLengthQString,parent);

    // Check if the DICOM object is a SQ Data element and extract the nested DICOM objects
    if( dcmElem && !dcmElem->isLeaf())
      {
      // now dcmElem points to a sequence of items
      ctkDICOMObjectModelPrivate::seqInsert(dicomItem, dynamic_cast<DcmSequenceOfItems*> (dcmElem), tagItem);
      }
  }
}

//------------------------------------------------------------------------------
void ctkDICOMObjectModelPrivate::seqInsert(ctkDICOMItem* dicomItem, DcmSequenceOfItems *dataset, QStandardItem *parent)
{
  DcmObject *dO = dataset->nextInContainer(NULL);

  for( ; dO; dO = dataset->nextInContainer(dO))
    {
    DcmElement *dcmElem = dynamic_cast<DcmElement *> (dO);
    QString tagValue = "";
    DcmTag tag = dO->getTag();
    DcmTag tagKey = tag.getXTag();
    if( tagKey == DCM_SequenceDelimitationItem
        || tagKey == DCM_ItemDelimitationItem)
      {
      return;
      }

    QString tagName = tag.getTagName();
    DcmTagKey tagX = tag.getXTag();
    QString tagHexName = tagX.toString().c_str();
    DcmVR VR = dO->getVR();
    QString VRName = VR.getVRName();

    // Getting length
    int elementLength;
    elementLength = dO->getLength();
    QString elementLengthQString = QString::number(elementLength);

    if( dcmElem)
      {
      tagValue = getTagValue(dicomItem, dcmElem);
      }

    QStandardItem *tagItem = populateModelRow(tagName,tagHexName,tagValue,VRName,elementLengthQString,parent);

   if( dcmElem && !dcmElem->isLeaf())
      {
      ctkDICOMObjectModelPrivate::seqInsert(dicomItem, dynamic_cast<DcmSequenceOfItems*> (dcmElem), tagItem);
      }
    else if( tag.getXTag() == DCM_Item)
      {
      itemInsert(dicomItem, dynamic_cast<DcmItem*> (dO), tagItem);
      }
   }
}

//------------------------------------------------------------------------------
QString ctkDICOMObjectModelPrivate::getTagValue(ctkDICOMItem* dicomItem, DcmElement *dcmElem)
{
  std::ostringstream value;
  OFString part;
  std::string sep;
  int mult = dcmElem->getVM();
  int pos;

  if( mult>1)
    {
    value << "[" << mult << "] ";
    }

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

  QString tagValue = dicomItem->Decode(dcmElem->getTag(), value.str().c_str());
  return tagValue;
}

//------------------------------------------------------------------------------
QStandardItem* ctkDICOMObjectModelPrivate::populateModelRow(const QString& tagName,
   const QString& tagHexName, const QString& tagValue,const QString& VRName,
   const QString& elementLengthQString, QStandardItem *parent)
{
  // Create items
  QStandardItem *VRItem = new QStandardItem( VRName);
  QStandardItem *tagItem = new QStandardItem( tagName);
  QStandardItem *tagHexItem = new QStandardItem( tagHexName);
  QStandardItem *lengthItem = new QStandardItem( elementLengthQString);
  QStandardItem *valItem = new QStandardItem( tagValue);

  VRItem->setFlags(VRItem->flags() & ~Qt::ItemIsEditable);
  tagItem->setFlags(tagItem->flags() & ~Qt::ItemIsEditable);
  tagHexItem->setFlags(tagHexItem->flags() & ~Qt::ItemIsEditable);
  lengthItem->setFlags(lengthItem->flags() & ~Qt::ItemIsEditable);
  valItem->setFlags(valItem->flags() & ~Qt::ItemIsEditable);

  // Insert items
  QList<QStandardItem *> modelRow;

  modelRow.append( tagHexItem);
  modelRow.append( tagItem);
  modelRow.append( valItem);
  modelRow.append( VRItem);
  modelRow.append( lengthItem);
  parent->appendRow( modelRow);

  return tagHexItem;
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
ctkDICOMObjectModel::~ctkDICOMObjectModel()
{
}

//------------------------------------------------------------------------------
void ctkDICOMObjectModel::setFile(const QString &fileName)
{
  Q_D(ctkDICOMObjectModel);

  OFCondition status = d->fileFormat.loadFile( fileName.toUtf8().data());
  if( !status.good() )
    {
    // TODO: Through an error message.
    }

  DcmDataset *dataset = d->fileFormat.getDataset();

  ctkDICOMItem* dicomItem = new ctkDICOMItem;
  dicomItem->InitializeFromItem(dataset);

  d->rootItem = this->invisibleRootItem();

  if(d->rootItem->hasChildren())
    {
    d->rootItem->removeRows(0, d->rootItem->rowCount());
    }

  d->itemInsert(dicomItem, dataset, d->rootItem);

  delete dicomItem;
}
