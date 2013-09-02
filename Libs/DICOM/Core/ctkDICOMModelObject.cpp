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
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstd.h"        /* for class OFStandard */

// CTK DICOM Core
#include "ctkDICOMModelObject.h"

//------------------------------------------------------------------------------
class ctkDICOMModelObjectPrivate
{
  
  Q_DECLARE_PUBLIC(ctkDICOMModelObject);
protected:
  ctkDICOMModelObject* const q_ptr;

public:
  ctkDICOMModelObjectPrivate(ctkDICOMModelObject&);
  virtual ~ctkDICOMModelObjectPrivate();
  void init();
  void ctkDICOMModelObjectPrivate::itemInsert( DcmItem *dataset, QStandardItem *parent);
  void ctkDICOMModelObjectPrivate::seqInsert( DcmSequenceOfItems *dataset, QStandardItem *parent);

  DcmFileFormat fileFormat;
  QStandardItem *rootItem;
};
//------------------------------------------------------------------------------
ctkDICOMModelObjectPrivate::ctkDICOMModelObjectPrivate(ctkDICOMModelObject& o):q_ptr(&o)
{

}

//------------------------------------------------------------------------------
ctkDICOMModelObjectPrivate::~ctkDICOMModelObjectPrivate()
{

}
//------------------------------------------------------------------------------
void ctkDICOMModelObjectPrivate::init()
{
Q_Q(ctkDICOMModelObject);
QStringList horizontalHeaderLabels;
horizontalHeaderLabels.append( QString("Tag"));
horizontalHeaderLabels.append( QString("Value"));
q->setHorizontalHeaderLabels(horizontalHeaderLabels);
	
}
//------------------------------------------------------------------------------
#undef max
#undef min
void ctkDICOMModelObjectPrivate::itemInsert( DcmItem *dataset, QStandardItem *parent)
{
  DcmStack stack;
  dataset->nextObject( stack, OFTrue);

  for( ; stack.top(); dataset->nextObject( stack, OFFalse))
    {
      DcmObject *dO =  stack.top();
      // DcmElement *dcmElem = dataset->getElement( idx);
      DcmElement *dcmElem = dynamic_cast<DcmElement *> (dO);
      QString tagValue = "";

      DcmTag tag = dO->getTag();
      QString tagName = tag.getTagName();
      if( tag.getXTag() == DCM_SequenceDelimitationItem
	  || tag.getXTag() == DCM_ItemDelimitationItem
	  || "Item" == tagName)
	{
	  return;
	}

      //      std::cerr << "node is dcmElem=" << dcmElem << "\n";

      if( dcmElem)
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
	  // TODO define max elem per line
	  for( pos=0; pos < std::min(mult,10); pos++)
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
	}
      // create items ...
      QStandardItem *tagItem = new QStandardItem( tagName);
      QStandardItem *valItem = new QStandardItem( tagValue);
      // ... and insert them
      QList<QStandardItem *> modelRow;
      modelRow.append( tagItem);
      modelRow.append( valItem);
      parent->appendRow( modelRow);


      //      std::cerr << "    "
      //		<< tagName.toStdString() << " " << tagValue.toStdString();


      if( dcmElem)
	{
	  // 	  std::cerr << " >> l=" << dcmElem->isLeaf() 
	  //		    << "  nx=" << dataset->nextInContainer( dcmElem);
	  if( !dcmElem->isLeaf())
	    {
	      // now dcmElem  points to a sequenceOfItems
	      ctkDICOMModelObjectPrivate::seqInsert( dynamic_cast<DcmSequenceOfItems*> (dcmElem), tagItem);	 
	    }
	}

      //      std::cerr  << "\n";
    }

}

//------------------------------------------------------------------------------
void ctkDICOMModelObjectPrivate::seqInsert( DcmSequenceOfItems *dataset, QStandardItem *parent)
{

  //  std::cerr << "Entering seqInsert" << "\n";
  DcmObject *dO = dataset->nextInContainer( NULL);

  //  std::cerr << "Entered nested level   d0=" << dO << "\n";
  //  std::cerr << "First node is dcmElem=" << dynamic_cast<DcmElement *> (dO) << "\n";

  for( ; dO; dO = dataset->nextInContainer(dO))
    {
      // DcmElement *dcmElem = dataset->getElement( idx);
      DcmElement *dcmElem = dynamic_cast<DcmElement *> (dO);
      QString tagValue = "";

      DcmTag tag = dO->getTag();
      if( tag.getXTag() == DCM_SequenceDelimitationItem
	  || tag.getXTag() == DCM_ItemDelimitationItem)
	{
	  return;
	}

      QString tagName = tag.getTagName();

      //      std::cerr << "node is dcmElem=" << dcmElem << "\n";

      if( dcmElem)
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
	  // TODO define max elem per line
	  for( pos=0; pos < std::min(mult,10); pos++)
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
	}
      // create items ...
      QStandardItem *tagItem = new QStandardItem( tagName);
      QStandardItem *valItem = new QStandardItem( tagValue);
      // ... and insert them
      QList<QStandardItem *> modelRow;
      modelRow.append( tagItem);
      modelRow.append( valItem);
      parent->appendRow( modelRow);


      //      std::cerr << "    "
      //		<< tagName.toStdString() << " " << tagValue.toStdString();


      if( dcmElem)
	{
	  // 	  std::cerr << " >> l=" << dcmElem->isLeaf() 
	  //		    << "  nx=" << dataset->nextInContainer( dcmElem);
	  if( !dcmElem->isLeaf())
	    {
	      // now dcmElem  points to a sequenceOfItems
			ctkDICOMModelObjectPrivate::seqInsert( dynamic_cast<DcmSequenceOfItems*> (dcmElem), tagItem);	 
	    }
	}
      else if( tag.getXTag() == DCM_Item)
	{
	      itemInsert( dynamic_cast<DcmItem*> (dO), tagItem);	 
	}
      //      std::cerr  << "\n";
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ctkDICOMModelObject::ctkDICOMModelObject(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkDICOMModelObjectPrivate(*this))
{
Q_D(ctkDICOMModelObject);
d->init();
  
}

//------------------------------------------------------------------------------
ctkDICOMModelObject::ctkDICOMModelObject(const ctkDICOMModelObject& other)
{
}

//------------------------------------------------------------------------------
ctkDICOMModelObject::~ctkDICOMModelObject()
{
}

//------------------------------------------------------------------------------

void ctkDICOMModelObject::setFile(const QString &fileName)
{
  Q_D(ctkDICOMModelObject);
 
  OFCondition status = d->fileFormat.loadFile( fileName.toLatin1().data());
  if( !status.good())
    {
		// TODO: Add through error
    }
  
  DcmDataset *dataset = d->fileFormat.getDataset();
  d->rootItem = ctkDICOMModelObject::invisibleRootItem();
  d->itemInsert( dataset, d->rootItem);
}
