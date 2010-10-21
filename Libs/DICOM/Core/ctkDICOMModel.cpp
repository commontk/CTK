/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

// Qt includes
#include <QStringList>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>

#include <QTime>
#include <QDebug>

// ctkDICOMCore includes
#include "ctkDICOMModel.h"
#include "ctkLogger.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMModel" );
struct Node;

//------------------------------------------------------------------------------
class ctkDICOMModelPrivate
{
  Q_DECLARE_PUBLIC(ctkDICOMModel);
protected:
  ctkDICOMModel* const q_ptr;
  
public:
  ctkDICOMModelPrivate(ctkDICOMModel&);
  virtual ~ctkDICOMModelPrivate();
  void init();

  enum IndexType{
    RootType,
    PatientType,
    StudyType,
    SeriesType,
    ImageType
  };
 
  void fetch(const QModelIndex& indexValue, int limit);
  Node* createNode(int row, const QModelIndex& parentValue)const;
  Node* nodeFromIndex(const QModelIndex& indexValue)const;
  //QModelIndexList indexListFromNode(const Node* node)const;
  //QModelIndexList modelIndexList(Node* node = 0)const;
  //int childrenCount(Node* node = 0)const;
  // move it in the Node struct
  QVariant value(Node* parentValue, int row, int field)const;
  QVariant value(const QModelIndex& indexValue, int row, int field)const;
  QString  generateQuery(const QString& fields, const QString& table, const QString& conditions = QString())const;
  void updateQueries(Node* node)const;

  Node*        RootNode;
  QSqlDatabase DataBase;
  QStringList  Headers;
  QString      Sort;
};

//------------------------------------------------------------------------------
// 1 node per row
struct Node
{
  ~Node()
    {
    foreach(Node* node, this->Children)
      {
      delete node;
      }
    this->Children.clear();
    }
  ctkDICOMModelPrivate::IndexType Type;
  Node*     Parent;
  QVector<Node*> Children;
  int       Row;
  QSqlQuery Query;
  QString   UID;
  int       RowCount;
  bool      AtEnd;
  bool      Fetching;
};

//------------------------------------------------------------------------------
ctkDICOMModelPrivate::ctkDICOMModelPrivate(ctkDICOMModel& o):q_ptr(&o)
{
  this->RootNode     = 0;
}

//------------------------------------------------------------------------------
ctkDICOMModelPrivate::~ctkDICOMModelPrivate()
{
  delete this->RootNode;
  this->RootNode = 0;
}

//------------------------------------------------------------------------------
void ctkDICOMModelPrivate::init()
{
  this->Headers = QStringList() << "Name" << "Age" << "Scan" << "Date" << "Subject ID"
                  << "Number" << "Institution" << "Referrer" << "Performer";
}

//------------------------------------------------------------------------------
Node* ctkDICOMModelPrivate::nodeFromIndex(const QModelIndex& indexValue)const
{
  return indexValue.isValid() ? reinterpret_cast<Node*>(indexValue.internalPointer()) : this->RootNode;
}

/*
//------------------------------------------------------------------------------
QModelIndexList ctkDICOMModelPrivate::indexListFromNode(const Node* node)const
{
  Q_Q(const ctkDICOMModel);
  Q_ASSERT(node);
  QModelIndexList indexList;
  
  Node* parentNode = node->Parent;
  if (parentNode == 0)
    {
    return indexList;
    }
  int field = parentNode->Query.record().indexOf("UID");
  int row = -1;
  for (row = 0; row < parentNode->RowCount; ++row)
    {
    QString uid = this->value(parentNode, row, field).toString();
    if (uid == node->UID)
      {
      break;
      }
    }
  if (row >= parentNode->RowCount)
    {
    return indexList;
    }
  for (int column = 0 ; column < this->Headers.size(); ++column)
    {
    indexList.append(q->createIndex(row, column, parentNode));
    }
  return indexList;
}

//------------------------------------------------------------------------------
QModelIndexList ctkDICOMModelPrivate::modelIndexList(Node* node)const
{
  QModelIndexList list;
  if (node == 0)
    {
    node = this->RootNode;
    }
  foreach(Node* child, node->Children)
    {
    list.append(this->indexListFromNode(child));
    }
  foreach(Node* child, node->Children)
    {
    list.append(this->modelIndexList(child));
    }
  return list;
}

//------------------------------------------------------------------------------
int ctkDICOMModelPrivate::childrenCount(Node* node)const
{
  int count = 0;
  if (node == 0)
    {
    node = this->RootNode;
    }
  count += node->Children.size();
  foreach(Node* child, node->Children)
    {
    count += this->childrenCount(child);
    }
  return count;
}
*/
//------------------------------------------------------------------------------
Node* ctkDICOMModelPrivate::createNode(int row, const QModelIndex& parentValue)const
{
  Node* node = new Node;
  Node* nodeParent = 0;
  if (row == -1)
    {// root node
    node->Type = ctkDICOMModelPrivate::RootType;
    node->Parent = 0;
    }
  else
    {
    nodeParent = this->nodeFromIndex(parentValue); 
    nodeParent->Children.push_back(node);
    node->Parent = nodeParent;
    node->Type = ctkDICOMModelPrivate::IndexType(nodeParent->Type + 1);
    }
  node->Row = row;
  if (node->Type != ctkDICOMModelPrivate::RootType)
    {
    int field = nodeParent->Query.record().indexOf("UID");
    node->UID = this->value(parentValue, row, field).toString();
    }
  
  node->RowCount = 0;
  node->AtEnd = false;
  node->Fetching = false;

  this->updateQueries(node);
  
  return node;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModelPrivate::value(const QModelIndex& parentValue, int row, int column) const
{
  Node* node = this->nodeFromIndex(parentValue);
  if (row >= node->RowCount)
    {      
    const_cast<ctkDICOMModelPrivate *>(this)->fetch(parentValue, row + 256);
    }
  return this->value(node, row, column);
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModelPrivate::value(Node* parentValue, int row, int column) const
{
  Q_ASSERT(row < parentValue->RowCount);

  if (!parentValue->Query.seek(row)) 
    {
    qDebug() << parentValue->Query.lastError();
    Q_ASSERT(parentValue->Query.seek(row));
    return QVariant();
    }
  QVariant res = parentValue->Query.value(column);
  Q_ASSERT(res.isValid());
  return res;
}

//------------------------------------------------------------------------------
QString ctkDICOMModelPrivate::generateQuery(const QString& fields, const QString& table, const QString& conditions)const
{
  QString res = QString("SELECT ") + fields + QString(" FROM ") + table;
  if (!conditions.isEmpty())
    {
    res += QString(" WHERE ") + conditions;
    }
  if (!this->Sort.isEmpty())
    {
    res += QString(" ORDER BY ") + this->Sort;
    }
  logger.debug ( "ctkDICOMModelPrivate::generateQuery: query is: " + res );
  return res;
}

//------------------------------------------------------------------------------
void ctkDICOMModelPrivate::updateQueries(Node* node)const
{
  // are you kidding me, it should be virtualized here :-)
  QString query;
  switch(node->Type)
    {
    default:
      Q_ASSERT(node->Type == ctkDICOMModelPrivate::RootType);
      break;
    case ctkDICOMModelPrivate::RootType:
      //query = QString("SELECT  FROM ");
      query = this->generateQuery("UID as UID, PatientsName as Name, PatientsAge as Age, PatientsBirthDate as Date, PatientID as \"Subject ID\"","Patients");
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Root: query is: " + query );
      break;
    case ctkDICOMModelPrivate::PatientType:
      //query = QString("SELECT  FROM Studies WHERE PatientsUID='%1'").arg(node->UID);
      query = this->generateQuery("StudyInstanceUID as UID, StudyDescription as Name, ModalitiesInStudy as Scan, StudyDate as Date, AccessionNumber as Number, ReferringPhysician as Institution, ReferringPhysician as Referrer, PerformingPhysiciansName as Performer", "Studies",QString("PatientsUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Patient: query is: " + query );
      break;
    case ctkDICOMModelPrivate::StudyType:
      //query = QString("SELECT SeriesInstanceUID as UID, SeriesDescription as Name, BodyPartExamined as Scan, SeriesDate as Date, AcquisitionNumber as Number FROM Series WHERE StudyInstanceUID='%1'").arg(node->UID);
      query = this->generateQuery("SeriesInstanceUID as UID, SeriesDescription as Name, BodyPartExamined as Scan, SeriesDate as Date, AcquisitionNumber as Number","Series",QString("StudyInstanceUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Study: query is: " + query );
      break;
    case ctkDICOMModelPrivate::SeriesType:
      //query = QString("SELECT Filename as UID, Filename as Name, SeriesInstanceUID as Date FROM Images WHERE SeriesInstanceUID='%1'").arg(node->UID);
      query = this->generateQuery("Filename as UID, Filename as Name, SeriesInstanceUID as Date", "Images", QString("SeriesInstanceUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Series: query is: " + query );
      break;
    case ctkDICOMModelPrivate::ImageType:
      break;
    }
  node->Query = QSqlQuery(query, this->DataBase);
  foreach(Node* child, node->Children)
    {
    this->updateQueries(child);
    }
}

//------------------------------------------------------------------------------
void ctkDICOMModelPrivate::fetch(const QModelIndex& indexValue, int limit)
{
  Q_Q(ctkDICOMModel);
  Node* node = this->nodeFromIndex(indexValue);
  if (node->AtEnd || limit <= node->RowCount || node->Fetching/*|| bottom.column() == -1*/)
    {
    return;
    }
  node->Fetching = true;

  int newRowCount;
  const int oldRowCount = node->RowCount;

  // try to seek directly
  if (node->Query.seek(limit - 1)) 
    {
    newRowCount = limit;
    } 
  else 
    {
    newRowCount = qMax(oldRowCount, 1);
    if (node->Query.seek(newRowCount - 1)) 
      {
      while (node->Query.next())
        {
        ++newRowCount;
        }
      } 
    else 
      {
      // empty or invalid query
      newRowCount = 0;
      }
    node->AtEnd = true; // this is the end.
    }
  if (newRowCount > 0 && newRowCount > node->RowCount) 
    {
    q->beginInsertRows(indexValue, node->RowCount, newRowCount - 1);
    node->RowCount = newRowCount;
    node->Fetching = false;
    q->endInsertRows();
    } 
  else 
    {
    node->RowCount = newRowCount;
    node->Fetching = false;
    }
}

//------------------------------------------------------------------------------
ctkDICOMModel::ctkDICOMModel(QObject* parentValue): d_ptr(new ctkDICOMModelPrivate(*this))
{
  Q_UNUSED(parentValue);
  Q_D(ctkDICOMModel);
  d->init();
}

//------------------------------------------------------------------------------
ctkDICOMModel::~ctkDICOMModel()
{
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::canFetchMore ( const QModelIndex & parentValue ) const
{
  Q_D(const ctkDICOMModel);
  Node* node = d->nodeFromIndex(parentValue);
  return !node->AtEnd;
}

//------------------------------------------------------------------------------
int ctkDICOMModel::columnCount ( const QModelIndex & _parent ) const
{
  Q_D(const ctkDICOMModel);
  Q_UNUSED(_parent);
  return d->RootNode != 0 ? d->Headers.size() : 0;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModel::data ( const QModelIndex & indexValue, int role ) const
{
  Q_D(const ctkDICOMModel);
  if (role & ~(Qt::DisplayRole | Qt::EditRole))
    {
    return QVariant();
    }
  QModelIndex indexParent = this->parent(indexValue);
  Node* parentNode = d->nodeFromIndex(indexParent);
  if (indexValue.row() >= parentNode->RowCount)
    {      
    const_cast<ctkDICOMModelPrivate *>(d)->fetch(indexValue, indexValue.row());
    }
/*
  if (!node->Query.seek(indexValue.row())) 
    {
    qDebug() << node->Query.lastError();
    return QVariant();
    }
    */
  int field = parentNode->Query.record().indexOf(d->Headers[indexValue.column()]);
  if (field < 0)
    {
    return QString();
    }
  return d->value(indexParent, indexValue.row(), field);
  //return node->Query.value(field);
}

//------------------------------------------------------------------------------
void ctkDICOMModel::fetchMore ( const QModelIndex & parentValue )
{
  Q_D(ctkDICOMModel);
  Node* node = d->nodeFromIndex(parentValue);
  d->fetch(parentValue, qMax(node->RowCount, 0) + 256);
}

//------------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMModel::flags ( const QModelIndex & indexValue ) const
{
  Q_UNUSED(indexValue);
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::hasChildren ( const QModelIndex & parentValue ) const
{
  Q_D(const ctkDICOMModel);
  if (parentValue.column() > 0)
    {
    return false;
    }
  Node* node = d->nodeFromIndex(parentValue);
  if (!node)
    {
    return false;
    }
  if (node->RowCount == 0 && !node->AtEnd)
    {
    //const_cast<qCTKDCMTKModelPrivate*>(d)->fetch(parentValue, 1);
    return node->Query.seek(0);
    }
  return node->RowCount > 0;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModel::headerData(int section, Qt::Orientation orientation, int role)const
{
  Q_D(const ctkDICOMModel);
  // @bug: this expression is not "valid", DisplayRole and EditRole are not bitmasks
  if (role & ~(Qt::DisplayRole | Qt::EditRole))
    {
    return QVariant();
    }
  if (orientation == Qt::Vertical)
    {
    return section;
    }
  Q_ASSERT(orientation == Qt::Horizontal);
  Q_ASSERT(section < d->Headers.size());
  return d->Headers[section];
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMModel::index ( int row, int column, const QModelIndex & parentValue ) const
{
  Q_D(const ctkDICOMModel);
  if (d->RootNode == 0 || parentValue.column() > 0)
    {
    return QModelIndex();
    }
  Node* parentNode = d->nodeFromIndex(parentValue);
  int field = parentNode->Query.record().indexOf("UID");
  QString uid = d->value(parentValue, row, field).toString();
  Node* node = 0;
  foreach(Node* tmpNode, parentNode->Children)
    {
    if (tmpNode->UID == uid)
      {
      node = tmpNode;
      break;
      }
    }
  if (node == 0)
    {
    node = d->createNode(row, parentValue);
    }
  return this->createIndex(row, column, node);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMModel::parent ( const QModelIndex & indexValue ) const
{
  Q_D(const ctkDICOMModel);
  Node* node = d->nodeFromIndex(indexValue);
  Q_ASSERT(node);
  Node* parentNode = node->Parent;
  if (parentNode == 0)
    {// node is root
    return QModelIndex();
    }
  return parentNode == d->RootNode ? QModelIndex() : this->createIndex(parentNode->Row, 0, parentNode);
  /* need to recalculate the parent row
  Node* greatParentNode = parentNode->Parent;
  if (greatParentNode == 0)
    {
    return QModelIndex();
    }
  int field = greatParentNode->Query.record().indexOf("UID");
  int row = -1;
  for (row = 0; row < greatParentNode->RowCount; ++row)
    {
    QString uid = d->value(greatParentNode, row, field).toString();
    if (uid == parentNode->UID)
      {
      break;
      }
    }
  Q_ASSERT(row < greatParentNode->RowCount);
  return this->createIndex(row, 0, parentNode);
  */
}

//------------------------------------------------------------------------------
int ctkDICOMModel::rowCount ( const QModelIndex & parentValue ) const
{
  Q_D(const ctkDICOMModel);
  if (d->RootNode == 0 || parentValue.column() > 0)
    {
    return 0;
    }
  Node* node = d->nodeFromIndex(parentValue);
  Q_ASSERT(node);
  if (node->RowCount == 0 && !node->AtEnd)
    {
    //const_cast<ctkDICOMModelPrivate*>(d)->fetch(parentValue, 256);
    }
  return node->RowCount;
}

//------------------------------------------------------------------------------
void ctkDICOMModel::setDatabase(const QSqlDatabase &db)
{
  Q_D(ctkDICOMModel);

  this->beginResetModel();
  d->DataBase = db;
  
  delete d->RootNode;
  d->RootNode = 0;

  if (d->DataBase.tables().empty())
    {
    //Q_ASSERT(d->DataBase.isOpen());
    this->endResetModel();
    return;
    }
    
  d->RootNode = d->createNode(-1, QModelIndex());
  
  this->endResetModel();

  // TODO, use hasQuerySize everywhere, not only in setDataBase()
  bool hasQuerySize = d->RootNode->Query.driver()->hasFeature(QSqlDriver::QuerySize);
  if (hasQuerySize && d->RootNode->Query.size() > 0) 
    {
    int newRowCount= d->RootNode->Query.size();
    beginInsertRows(QModelIndex(), 0, qMax(0, newRowCount - 1));
    d->RootNode->RowCount = newRowCount;
    d->RootNode->AtEnd = true;
    endInsertRows();
    }
  d->fetch(QModelIndex(), 256);
}

//------------------------------------------------------------------------------
void ctkDICOMModel::sort(int column, Qt::SortOrder order)
{
  Q_D(ctkDICOMModel);
  /* The following would work if there is no fetch involved.
     ORDER BY doesn't just apply on the fetched item. By sorting
     new items can show up in the model, and we need to be more
     careful
  emit layoutAboutToBeChanged();
  QModelIndexList oldIndexList = d->modelIndexList();
  d->Sort = QString("\"%1\" %2")
    .arg(d->Headers[column])
    .arg(order == Qt::AscendingOrder ? "ASC" : "DESC");
  d->updateQueries(d->RootNode);
  QModelIndexList newIndexList = d->modelIndexList();
  Q_ASSERT(oldIndexList.count() == newIndexList.count());
  this->changePersistentIndexList(oldIndexList, newIndexList);
  emit layoutChanged();
  */
  this->beginResetModel();
  delete d->RootNode;
  d->RootNode = 0;
  d->Sort = QString("\"%1\" %2")
    .arg(d->Headers[column])
    .arg(order == Qt::AscendingOrder ? "ASC" : "DESC");
  d->RootNode = d->createNode(-1, QModelIndex());
  
  this->endResetModel();
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
  Q_D(ctkDICOMModel);
  if (role & ~(Qt::DisplayRole | Qt::EditRole))
    {
    return false;
    }
  if (orientation == Qt::Vertical)
    {
    return false;
    }
  if (value.toString() == d->Headers[section])
    {
    return false;
    }
  d->Headers[section] = value.toString();
  emit this->headerDataChanged(orientation, section, section);
  return true;
}
