/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

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
#include <QSqlRecord>
#include <QSqlResult>

#include <QTime>
#include <QDebug>

// ctkDICOMCore includes
#include "ctkDICOMModel.h"
#include "ctkLogger.h"

static ctkLogger logger ( "org.commontk.dicom.DICOMModel" );
struct Node;

Q_DECLARE_METATYPE(Qt::CheckState);
Q_DECLARE_METATYPE(QStringList);

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
  QList<QMap<int, QVariant> > Headers;
  QString      Sort;
  QMap<QString, QVariant> SearchParameters;

  ctkDICOMModel::IndexType StartLevel;
  ctkDICOMModel::IndexType EndLevel;
};

//------------------------------------------------------------------------------
// 1 node per row
// TBD: should probably use the QStandardItems instead.
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
  ctkDICOMModel::IndexType Type;
  Node*                           Parent;
  QVector<Node*>                  Children;
  int                             Row;
  QSqlQuery                       Query;
  QString                         UID;
  int                             RowCount;
  bool                            AtEnd;
  bool                            Fetching;
  QMap<int, QVariant>             Data;
};

//------------------------------------------------------------------------------
ctkDICOMModelPrivate::ctkDICOMModelPrivate(ctkDICOMModel& o):q_ptr(&o)
{
  this->RootNode     = 0;
  this->StartLevel = ctkDICOMModel::RootType;
  this->EndLevel = ctkDICOMModel::ImageType;
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
  QMap<int, QVariant> data;
  data[Qt::DisplayRole] = QString("Name");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Age");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Scan");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Date");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Subject ID");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Number");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Institution");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Referrer");
  this->Headers << data;
  data[Qt::DisplayRole] = QString("Performer");
  this->Headers << data;
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
    node->Type = ctkDICOMModel::RootType;
    node->Parent = 0;
#if CHECKABLE_COLUMNS
    // CHECKABLE_COLUMNS are disabled by default - they are not yet used by other
    // parts of the ctkDICOM infrastructure so they are misleading to the user
    node->Data[Qt::CheckStateRole] = Qt::Unchecked;
#endif
    }
  else
    {
    nodeParent = this->nodeFromIndex(parentValue); 
    nodeParent->Children.push_back(node);
    node->Parent = nodeParent;
    node->Type = ctkDICOMModel::IndexType(nodeParent->Type + 1);
    }
  node->Row = row;
  if (node->Type != ctkDICOMModel::RootType)
    {
    int field = 0;//nodeParent->Query.record().indexOf("UID");
    node->UID = this->value(parentValue, row, field).toString();
#if CHECKABLE_COLUMNS
    node->Data[Qt::CheckStateRole] = node->Parent->Data[Qt::CheckStateRole];
#endif
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
QVariant ctkDICOMModelPrivate::value(Node* parentNode, int row, int column) const
{
  if (row < 0 || column < 0 || !parentNode || row >= parentNode->RowCount)
    {
    return QVariant();
    }
  
  if (!parentNode->Query.seek(row))
    {
    qDebug() << parentNode->Query.lastError();
    Q_ASSERT(parentNode->Query.seek(row));
    return QVariant();
    }
  QVariant res = parentNode->Query.value(column);
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
  QString condition;
  switch(node->Type)
    {
    default:
      Q_ASSERT(node->Type == ctkDICOMModel::RootType);
      break;
    case ctkDICOMModel::RootType:
      //query = QString("SELECT  FROM ");
      if(this->SearchParameters["Name"].toString() != ""){
        condition.append("PatientsName LIKE \"%" + this->SearchParameters["Name"].toString() + "%\"");
      }
      query = this->generateQuery("UID as UID, PatientsName as Name, PatientsAge as Age, PatientsBirthDate as Date, PatientID as \"Subject ID\"","Patients", condition);
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Root: query is: " + query );
      break;
    case ctkDICOMModel::PatientType:
      //query = QString("SELECT  FROM Studies WHERE PatientsUID='%1'").arg(node->UID);
      if(this->SearchParameters["Study"].toString() != "")
        {
        condition.append("StudyDescription LIKE \"%" + this->SearchParameters["Study"].toString() + "%\"" + " AND ");
        }
      if(this->SearchParameters["Modalities"].value<QStringList>().count() > 0)
        {
        condition.append("ModalitiesInStudy IN (\"" + this->SearchParameters["Modalities"].value<QStringList>().join("\",\"") + "\") AND ");
        }
      if(this->SearchParameters["StartDate"].toString() != "" &&
         this->SearchParameters["EndDate"].toString() != "")
        {
          condition.append(" ( StudyDate BETWEEN \'" + QDate::fromString(this->SearchParameters["StartDate"].toString(), "yyyyMMdd").toString("yyyy-MM-dd")
                           + "\' AND \'" + QDate::fromString(this->SearchParameters["EndDate"].toString(), "yyyyMMdd").toString("yyyy-MM-dd") + "\' ) AND ");
        }
      query = this->generateQuery("StudyInstanceUID as UID, StudyDescription as Name, ModalitiesInStudy as Scan, StudyDate as Date, AccessionNumber as Number, ReferringPhysician as Institution, ReferringPhysician as Referrer, PerformingPhysiciansName as Performer", "Studies", condition + QString("PatientsUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Patient: query is: " + query );
      break;
    case ctkDICOMModel::StudyType:
      //query = QString("SELECT SeriesInstanceUID as UID, SeriesDescription as Name, BodyPartExamined as Scan, SeriesDate as Date, AcquisitionNumber as Number FROM Series WHERE StudyInstanceUID='%1'").arg(node->UID);
      if(this->SearchParameters["Series"].toString() != "")
        {
        condition.append("SeriesDescription LIKE \"%" + this->SearchParameters["Series"].toString() + "%\"" + " AND ");
        }
      query = this->generateQuery("SeriesInstanceUID as UID, SeriesDescription as Name, Modality as Age, SeriesNumber as Scan, BodyPartExamined as \"Subject ID\", SeriesDate as Date, AcquisitionNumber as Number","Series",condition + QString("StudyInstanceUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Study: query is: " + query );
      break;
    case ctkDICOMModel::SeriesType:
      if(this->SearchParameters["ID"].toString() != "")
        {
        condition.append("SOPInstanceUID LIKE \"%" + this->SearchParameters["ID"].toString() + "%\"" + " AND ");
        }
      //query = QString("SELECT Filename as UID, Filename as Name, SeriesInstanceUID as Date FROM Images WHERE SeriesInstanceUID='%1'").arg(node->UID);
      query = this->generateQuery("SOPInstanceUID as UID, Filename as Name, SeriesInstanceUID as Date", "Images", condition + QString("SeriesInstanceUID='%1'").arg(node->UID));
      logger.debug ( "ctkDICOMModelPrivate::updateQueries for Series: query is: " + query );
      break;
    case ctkDICOMModel::ImageType:
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
ctkDICOMModel::ctkDICOMModel(QObject* parentObject)
  : Superclass(parentObject)
  , d_ptr(new ctkDICOMModelPrivate(*this))
{
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
  return node ? !node->AtEnd : false;
}

//------------------------------------------------------------------------------
int ctkDICOMModel::columnCount ( const QModelIndex & _parent ) const
{
  Q_D(const ctkDICOMModel);
  Q_UNUSED(_parent);
  return d->RootNode != 0 ? d->Headers.size() : 0;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModel::data ( const QModelIndex & dataIndex, int role ) const
{
  Q_D(const ctkDICOMModel);
  if ( role == UIDRole )
    {
    Node* node = d->nodeFromIndex(dataIndex);
    return node ? node->UID : QString() ;
    }
  else if ( role == TypeRole )
    {
    Node* node = d->nodeFromIndex(dataIndex);
    return node ? node->Type : 0;
    }
  else if ( dataIndex.column() == 0 && role == Qt::CheckStateRole)
    {
    Node* node = d->nodeFromIndex(dataIndex);
    return node ? node->Data[Qt::CheckStateRole] : 0;
    }


  if (role != Qt::DisplayRole && role != Qt::EditRole)
    {
    if (dataIndex.column() != 0)
      {
      return QVariant();
      }
    Node* node = d->nodeFromIndex(dataIndex);
    if (!node)
      {
      return QVariant();
      }
    return node->Data[role];
    }
  QModelIndex parentIndex = this->parent(dataIndex);
  Node* parentNode = d->nodeFromIndex(parentIndex);
  if (dataIndex.row() >= parentNode->RowCount)
    {      
    const_cast<ctkDICOMModelPrivate *>(d)->fetch(dataIndex, dataIndex.row());
    }
  QString columnName = d->Headers[dataIndex.column()][Qt::DisplayRole].toString();
  int field = parentNode->Query.record().indexOf(columnName);
  if (field < 0)
    {
    // Not all the columns are in the record, it's ok to have no field here.
    // Return an empty string in that case (not a QVariant() that means it's
    // invalid).
    return QString();
    }
  return d->value(parentIndex, dataIndex.row(), field);
}

//------------------------------------------------------------------------------
void ctkDICOMModel::fetchMore ( const QModelIndex & parentValue )
{
  Q_D(ctkDICOMModel);
  Node* node = d->nodeFromIndex(parentValue);
  d->fetch(parentValue, qMax(node->RowCount, 0) + 256);
}

//------------------------------------------------------------------------------
Qt::ItemFlags ctkDICOMModel::flags ( const QModelIndex & modelIndex ) const
{
  Q_D(const ctkDICOMModel);
  Qt::ItemFlags indexFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  if (modelIndex.column() != 0)
    {
    return indexFlags;
    }
  Node* node = d->nodeFromIndex(modelIndex);
  if (!node)
    {
    return indexFlags;
    }
  bool checkable = true;
  node->Data[Qt::CheckStateRole].toInt(&checkable);
  indexFlags = indexFlags | (checkable ? Qt::ItemIsUserCheckable : Qt::NoItemFlags);
  return indexFlags;
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::hasChildren ( const QModelIndex & parentIndex ) const
{
  Q_D(const ctkDICOMModel);
  // only items in the first columns have index, shortcut the following for
  // speed issues.
  if (parentIndex.column() > 0)
    {
    return false;
    }
  Node* node = d->nodeFromIndex(parentIndex);
  if (!node)
    {
    return false;
    }

  // We want to show only until EndLevel
  if(node->Type >= d->EndLevel)return false;

  // It's not because we don't have row that we don't have children, maybe it
  // just means that the children haven't been fetched yet
  if (node->RowCount == 0 && !node->AtEnd)
    {
    // We don't want to fetch the data because we don't want to add children
    // to the index yet (it would be a mess to add rows inside a hasChildren)
    //const_cast<qCTKDCMTKModelPrivate*>(d)->fetch(parentIndex, 1);
    bool res = node->Query.seek(0);
    if (!res)
      {
      // now we know there is no children to the node, don't try next time.
      node->AtEnd = true;
      }
    return res;
    }
  return node->RowCount > 0;
}

//------------------------------------------------------------------------------
QVariant ctkDICOMModel::headerData(int section, Qt::Orientation orientation, int role)const
{
  Q_D(const ctkDICOMModel);
  if (orientation == Qt::Vertical)
    {
    if (role != Qt::DisplayRole)
      {
      return QVariant();
      }
    return section;
    }
  if (section < 0 || section >= d->Headers.size())
    {
    return QVariant();
    }
  return d->Headers[section][role];
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMModel::index ( int row, int column, const QModelIndex & parentIndex ) const
{
  Q_D(const ctkDICOMModel);
  // only the first column has children
  if (d->RootNode == 0 || parentIndex.column() > 0)
    {
    return QModelIndex();
    }
  Node* parentNode = d->nodeFromIndex(parentIndex);
  int field = 0;// always 0//parentNode->Query.record().indexOf("UID");
  QString uid = d->value(parentIndex, row, field).toString();
  Node* node = 0;
  foreach(Node* tmpNode, parentNode->Children)
    {
    if (tmpNode->UID == uid)
      {
      node = tmpNode;
      break;
      }
    }
  // TODO: Here it is assumed that ctkDICOMModel::index is called with valid
  // arguments, we should probably be a bit more careful.
  if (node == 0)
    {
    node = d->createNode(row, parentIndex);
    }
  return this->createIndex(row, column, node);
}

//------------------------------------------------------------------------------
QModelIndex ctkDICOMModel::parent ( const QModelIndex & indexValue ) const
{
  Q_D(const ctkDICOMModel);
  if (!indexValue.isValid())
    {
    return QModelIndex();
    }
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
  // Returns the amount of rows currently cached on the client.
  return node ? node->RowCount : 0;
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  Q_D(const ctkDICOMModel);
  if (role != Qt::CheckStateRole)
    {
    return false;
    }
  Node* node = d->nodeFromIndex(index);
  if (!node || node->Data[role] == value)
    {
    return false;
    }
  node->Data[role] = value;
  emit dataChanged(index, index);

  for(int i=0; i<node->Children.count(); i++)
    {
      this->setChildData(index.child(i,0), value, role);
    }

  if(index.parent().isValid())
    {
    this->setParentData(index.parent(), value, role);
    }

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::setChildData(const QModelIndex &index, const QVariant &value, int role)
{
  Q_D(const ctkDICOMModel);
  if (role != Qt::CheckStateRole)
    {
    return false;
    }
  Node* node = d->nodeFromIndex(index);
  if (!node || node->Data[role] == value)
    {
    return false;
    }
  node->Data[role] = value;
  emit dataChanged(index, index);

  for(int i=0; i<node->Children.count(); i++)
    {
      this->setData(index.child(i,0), value, role);
    }

  return true;
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::setParentData(const QModelIndex &index, const QVariant &value, int role)
{
  Q_D(const ctkDICOMModel);

  if(!index.isValid()){
    return false;
  }

  if (role != Qt::CheckStateRole)
    {
    return false;
    }
  else
    {
    bool checkedExist = false;
    bool partiallyCheckedExist = false;
    bool uncheckedExist = false;

    for(int i=0; i<index.model()->rowCount(index); i++)
      {
      Node* childNode = d->nodeFromIndex(index.child(i,0));
      if(childNode->Data[Qt::CheckStateRole].toUInt() == Qt::Checked)
        {
        checkedExist = true;
        }
      else if(childNode->Data[Qt::CheckStateRole].toUInt() ==  Qt::PartiallyChecked)
        {
        partiallyCheckedExist = true;
        }
      else if(childNode->Data[Qt::CheckStateRole].toUInt() ==  Qt::Unchecked)
        {
        uncheckedExist = true;
        }
      }

#ifdef CHECKABLE_COLUMNS
    if(partiallyCheckedExist || (checkedExist && uncheckedExist))
      {
      node->Data[Qt::CheckStateRole].toUInt() = Qt::PartiallyChecked;
      }
    else if(checkedExist)
      {
      node->Data[Qt::CheckStateRole].toUInt() = Qt::Checked;
      }
    else if(uncheckedExist)
      {
      node->Data[Qt::CheckStateRole].toUInt() = Qt::Unchecked;
      }
    else
      {
      node->Data[Qt::CheckStateRole].toUInt() = Qt::Unchecked;
      }
#endif

    emit dataChanged(index, index);

    this->setParentData(index.parent(), value, role);
    }
  return true;
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
void ctkDICOMModel::setDatabase(const QSqlDatabase &db,const QMap<QString, QVariant>& parameters)
{
  Q_D(ctkDICOMModel);

  this->beginResetModel();
  d->DataBase = db;
  d->SearchParameters = parameters;

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
ctkDICOMModel::IndexType  ctkDICOMModel::endLevel()const
{
  Q_D(const ctkDICOMModel);
  return d->EndLevel;
}

//------------------------------------------------------------------------------
void ctkDICOMModel::setEndLevel(ctkDICOMModel::IndexType level)
{
  Q_D(ctkDICOMModel);
  d->EndLevel = level;
}

//------------------------------------------------------------------------------
void ctkDICOMModel::reset()
{
  Q_D(ctkDICOMModel);
  // this could probably be done in a more elegant way
  this->setDatabase(d->DataBase);
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
    .arg(d->Headers[column][Qt::DisplayRole].toString())
    .arg(order == Qt::AscendingOrder ? "ASC" : "DESC");
  d->RootNode = d->createNode(-1, QModelIndex());
  
  this->endResetModel();
}

//------------------------------------------------------------------------------
bool ctkDICOMModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
  Q_D(ctkDICOMModel);
  if (orientation == Qt::Vertical)
    {
    return false;
    }
  if (section < 0 || section >= d->Headers.size() ||
      d->Headers[section][role] == value)
    {
    return false;
    }
  d->Headers[section][role] = value;
  emit this->headerDataChanged(orientation, section, section);
  return true;
}
