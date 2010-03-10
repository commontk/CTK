#include "qCTKDCMTKModel.h"

#include <QSqlQueryModel>
#include <QStringList>

struct Node
{
  Node* Parent;
  int   Row;
  int   Column;
};

class qCTKDCMTKModelPrivate:public qCTKPrivate<qCTKDCMTKModel>
{
public:
  qCTKDCMTKModelPrivate();
  ~qCTKDCMTKModelPrivate();
  void init();

  enum IndexType{
    RootType,
    PatientType,
    StudyType,
    SeriesType,
    ImageType
  };
  IndexType indexType(const QModelIndex& index)const;

  void updateRootModel(const QModelIndex& index);
  void updatePatientModel(const QModelIndex& index);
  void updateStudyModel(const QModelIndex& index);
  void updateSeriesModel(const QModelIndex& index);

  QModelIndex indexInRootQuery(const QModelIndex& index)const;
  QModelIndex indexInPatientQuery(const QModelIndex& index)const;
  QModelIndex indexInStudyQuery(const QModelIndex& index)const;
  QModelIndex indexInSeriesQuery(const QModelIndex& index)const;
  
  QSqlDatabase    DataBase;
  QSqlQueryModel* RootModel;
  QSqlQueryModel* PatientModel;
  QSqlQueryModel* StudyModel;
  QSqlQueryModel* SeriesModel;

  Node*           RootNode;
  Node*           PatientNode;
  Node*           StudyNode;
  Node*           SeriesNode;
  mutable QList<Node*>    Nodes;
};

qCTKDCMTKModelPrivate::qCTKDCMTKModelPrivate()
  :DataBase(QSqlDatabase::addDatabase("QSQLITE"))
{
  this->RootModel    = 0;
  this->PatientModel = 0;
  this->StudyModel   = 0;
  this->SeriesModel  = 0;
  this->RootNode     = 0;
  this->PatientNode  = 0;
  this->StudyNode    = 0;
  this->SeriesNode   = 0;
}

qCTKDCMTKModelPrivate::~qCTKDCMTKModelPrivate()
{
  foreach(Node* node, this->Nodes)
    {
    delete node;
    }
  this->Nodes.clear();

}

void qCTKDCMTKModelPrivate::init()
{
  QCTK_P(qCTKDCMTKModel);
  this->RootModel = new QSqlQueryModel(p);
  this->PatientModel = new QSqlQueryModel(p);
  this->StudyModel = new QSqlQueryModel(p);
  this->SeriesModel = new QSqlQueryModel(p);

  QObject::connect(this->RootModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
          p, SLOT(rootRowsAboutToBeInserted(const QModelIndex&, int, int)));
  QObject::connect(this->PatientModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
          p, SLOT(patientRowsAboutToBeInserted(const QModelIndex&, int, int)));
  QObject::connect(this->StudyModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
          p, SLOT(studyRowsAboutToBeInserted(const QModelIndex&, int, int)));
  QObject::connect(this->SeriesModel, SIGNAL(rowsAboutToBeInserted(const QModelIndex&, int, int)),
          p, SLOT(seriesRowsAboutToBeInserted(const QModelIndex&, int, int)));

  QObject::connect(this->RootModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
          p, SLOT(rootRowsInserted(const QModelIndex&, int, int)));
  QObject::connect(this->PatientModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
          p, SLOT(patientRowsInserted(const QModelIndex&, int, int)));
  QObject::connect(this->StudyModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
          p, SLOT(studyRowsInserted(const QModelIndex&, int, int)));
  QObject::connect(this->SeriesModel, SIGNAL(rowsInserted(const QModelIndex&, int, int)),
          p, SLOT(seriesRowsInserted(const QModelIndex&, int, int)));

  QObject::connect(this->RootModel, SIGNAL(modelAboutToBeReset()),
          p, SLOT(onModelAboutToBeReset()));
  QObject::connect(this->PatientModel, SIGNAL(modelAboutToBeReset()),
          p, SLOT(onModelAboutToBeReset()));
  QObject::connect(this->StudyModel, SIGNAL(modelAboutToBeReset()),
          p, SLOT(onModelAboutToBeReset()));
  QObject::connect(this->SeriesModel, SIGNAL(modelAboutToBeReset()),
          p, SLOT(onModelAboutToBeReset()));

  QObject::connect(this->RootModel, SIGNAL(modelReset()),
          p, SLOT(onModelReset()));
  QObject::connect(this->PatientModel, SIGNAL(modelReset()),
          p, SLOT(onModelReset()));
  QObject::connect(this->StudyModel, SIGNAL(modelReset()),
          p, SLOT(onModelReset()));
  QObject::connect(this->SeriesModel, SIGNAL(modelReset()),
          p, SLOT(onModelReset()));

  QObject::connect(this->RootModel, SIGNAL(layoutAboutToBeChanged()),
          p, SLOT(onLayoutAboutToBeChanged()));
  QObject::connect(this->PatientModel, SIGNAL(layoutAboutToBeChanged()),
          p, SLOT(onLayoutAboutToBeChanged()));
  QObject::connect(this->StudyModel, SIGNAL(layoutAboutToBeChanged()),
          p, SLOT(onLayoutAboutToBeChanged()));
  QObject::connect(this->SeriesModel, SIGNAL(layoutAboutToBeChanged()),
          p, SLOT(onLayoutAboutToBeChanged()));

  QObject::connect(this->RootModel, SIGNAL(layoutChanged()),
          p, SLOT(onLayoutChanged()));
  QObject::connect(this->PatientModel, SIGNAL(layoutChanged()),
          p, SLOT(onLayoutChanged()));
  QObject::connect(this->StudyModel, SIGNAL(layoutChanged()),
          p, SLOT(onLayoutChanged()));
  QObject::connect(this->SeriesModel, SIGNAL(layoutChanged()),
          p, SLOT(onLayoutChanged()));

  this->RootNode = new Node;
  this->RootNode->Parent = 0;
  this->RootNode->Row = -1;
  this->RootNode->Column = -1;
  this->Nodes.append(this->RootNode);
}

qCTKDCMTKModelPrivate::IndexType qCTKDCMTKModelPrivate::indexType(const QModelIndex& index)const
{
  QCTK_P(const qCTKDCMTKModel);
  if (!index.isValid())
    {
    return RootType;
    }
  QModelIndex indexParent = p->parent(index);
  if (!indexParent.isValid())
    {
    return PatientType;
    }
  indexParent = p->parent(indexParent);
  if (!indexParent.isValid())
    {
    return StudyType;
    }
  indexParent = p->parent(indexParent);
  if (!indexParent.isValid())
    {
    return SeriesType;
    }
  Q_ASSERT(!p->parent(indexParent).isValid());
  return ImageType;
}

QModelIndex qCTKDCMTKModelPrivate::indexInRootQuery(const QModelIndex& index) const
{
  return index;
}

QModelIndex qCTKDCMTKModelPrivate::indexInPatientQuery(const QModelIndex& index) const
{
  return index;
}

QModelIndex qCTKDCMTKModelPrivate::indexInStudyQuery(const QModelIndex& index) const
{
  return index;
}

QModelIndex qCTKDCMTKModelPrivate::indexInSeriesQuery(const QModelIndex& index) const
{
  return index;
}

void qCTKDCMTKModelPrivate::updateRootModel(const QModelIndex& index)
{
  this->RootModel->setQuery( "SELECT * FROM Patients", this->DataBase);
}

void qCTKDCMTKModelPrivate::updatePatientModel(const QModelIndex& index)
{
  QCTK_P(qCTKDCMTKModel);
  this->PatientNode = reinterpret_cast<Node*>(index.internalPointer());
  QString patientId = p->data(index).toString();
  this->PatientModel->setQuery( QString("SELECT * FROM Studies WHERE PatientsUID='%1'").arg(patientId), this->DataBase);
}

void qCTKDCMTKModelPrivate::updateStudyModel(const QModelIndex& index)
{
  QCTK_P(qCTKDCMTKModel);
  this->StudyNode = reinterpret_cast<Node*>(index.internalPointer());
  QString studyId = p->data(index).toString();
  this->StudyModel->setQuery( QString("SELECT * FROM Series WHERE StudyInstanceUID='%1'").arg(studyId), this->DataBase);
}

void qCTKDCMTKModelPrivate::updateSeriesModel(const QModelIndex& index)
{
  QCTK_P(qCTKDCMTKModel);
  this->SeriesNode = reinterpret_cast<Node*>(index.internalPointer());
  QString seriesId = p->data(index).toString();
  this->SeriesModel->setQuery( QString("SELECT * FROM Images WHERE SeriesInstanceUID='%1'").arg(seriesId), this->DataBase);
}

qCTKDCMTKModel::qCTKDCMTKModel(QObject* parent)
{
}

qCTKDCMTKModel::~qCTKDCMTKModel()
{
}

bool qCTKDCMTKModel::canFetchMore ( const QModelIndex & parent ) const
{
  QCTK_D(const qCTKDCMTKModel);
  switch(d->indexType(parent))
    {
    case qCTKDCMTKModelPrivate::RootType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateRootModel(parent);
      return d->RootModel->canFetchMore();
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updatePatientModel(parent);
      return d->PatientModel->canFetchMore();
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateStudyModel(parent);
      return d->StudyModel->canFetchMore();
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateSeriesModel(parent);
      return d->SeriesModel->canFetchMore();
      break;
    case qCTKDCMTKModelPrivate::ImageType:
    default:
      break;
    }
  return false;
}

int qCTKDCMTKModel::columnCount ( const QModelIndex & _parent ) const
{
  Q_UNUSED(_parent);
  return 8;
}

QVariant qCTKDCMTKModel::data ( const QModelIndex & index, int role ) const
{
  QCTK_D(const qCTKDCMTKModel);
  QVariant res;
  QModelIndex indexParent = this->parent(index);
  switch(d->indexType(index))
    {
    default:
    case qCTKDCMTKModelPrivate::RootType:
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateRootModel(indexParent);
      res = d->RootModel->data(d->indexInRootQuery(index), role);
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updatePatientModel(indexParent);
      res = d->PatientModel->data(d->indexInPatientQuery(index), role);
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateStudyModel(indexParent);
      res = d->StudyModel->data(d->indexInStudyQuery(index), role);
      break;
    case qCTKDCMTKModelPrivate::ImageType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateSeriesModel(indexParent);
      res = d->SeriesModel->data(d->indexInSeriesQuery(index), role);
      break;
    }
  return res;
}

void qCTKDCMTKModel::fetchMore ( const QModelIndex & parent )
{
  QCTK_D(qCTKDCMTKModel);
  switch(d->indexType(parent))
    {
    case qCTKDCMTKModelPrivate::RootType:
      d->updateRootModel(parent);
      d->RootModel->fetchMore(d->indexInRootQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      d->updatePatientModel(parent);
      d->PatientModel->fetchMore(d->indexInPatientQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      d->updateStudyModel(parent);
      d->StudyModel->fetchMore(d->indexInStudyQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      d->updateSeriesModel(parent);
      d->SeriesModel->fetchMore(d->indexInSeriesQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::ImageType:
    default:
      break;
    }
}

Qt::ItemFlags qCTKDCMTKModel::flags ( const QModelIndex & index ) const
{
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool qCTKDCMTKModel::hasChildren ( const QModelIndex & parent ) const
{
  QCTK_D(const qCTKDCMTKModel);
  switch(d->indexType(parent))
    {
    case qCTKDCMTKModelPrivate::RootType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateRootModel(parent);
      return d->RootModel->rowCount(d->indexInRootQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updatePatientModel(parent);
      return d->PatientModel->rowCount(d->indexInPatientQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateStudyModel(parent);
      return d->StudyModel->rowCount(d->indexInStudyQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateSeriesModel(parent);
      return d->SeriesModel->rowCount(d->indexInSeriesQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::ImageType:
    default:
      break;
    }
  return false;
}

QModelIndex qCTKDCMTKModel::index ( int row, int column, const QModelIndex & parent ) const
{
  QCTK_D(const qCTKDCMTKModel);
  QSqlQueryModel* res = 0;
  switch(d->indexType(parent))
    {
    case qCTKDCMTKModelPrivate::RootType:
      res = d->RootModel;
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      res = d->PatientModel;
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      res = d->StudyModel;
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      res = d->SeriesModel;
      break;
    case qCTKDCMTKModelPrivate::ImageType:
    default:
      Q_ASSERT(d->indexType(parent) != qCTKDCMTKModelPrivate::ImageType);
      break;
    }
  Node* parentNode = 
    reinterpret_cast<Node*>(parent.internalPointer());
  Node* node = 0;
  foreach(Node* tmpNode, d->Nodes)
    {
    if (tmpNode->Parent == parentNode &&
        tmpNode->Row == row && 
        tmpNode->Column == column)
      {
      node = tmpNode;
      break;
      }
    }
  if ( node == 0)
    {
    node = new Node;
    node->Parent = parentNode;
    node->Row = row;
    node->Column = column;
    d->Nodes.append(node);
    }
  return this->createIndex(row, column, node);
}

QModelIndex qCTKDCMTKModel::parent ( const QModelIndex & index ) const
{
  Node* node = 
    reinterpret_cast<Node*>(index.internalPointer());
  if (node == 0 || node->Parent == 0)
    {
    return this->createIndex(-1, -1, 0);
    }
  return this->createIndex(node->Parent->Row, node->Parent->Column, node->Parent);
}

int qCTKDCMTKModel::rowCount ( const QModelIndex & parent ) const
{
  QCTK_D(const qCTKDCMTKModel);
  int res = 0;
  switch(d->indexType(parent))
    {
    default:
    case qCTKDCMTKModelPrivate::RootType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateRootModel(parent);
      res = d->RootModel->rowCount(d->indexInRootQuery(parent));
      break;
      break;
    case qCTKDCMTKModelPrivate::PatientType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updatePatientModel(parent);
      res = d->PatientModel->rowCount(d->indexInPatientQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::StudyType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateStudyModel(parent);
      res = d->StudyModel->rowCount(d->indexInStudyQuery(parent));
      break;
    case qCTKDCMTKModelPrivate::SeriesType:
      const_cast<qCTKDCMTKModelPrivate*>(d)->updateSeriesModel(parent);
      res = d->SeriesModel->rowCount(d->indexInSeriesQuery(parent));
      break;
    }
  return res;
}

void qCTKDCMTKModel::setDataBase(const QString &db)
{
  QCTK_D(qCTKDCMTKModel);

  this->beginResetModel();
  d->DataBase.setDatabaseName(db);
  this->endResetModel();
  if (!d->DataBase.open() || d->DataBase.tables().empty())
    {
    //Q_ASSERT(d->DataBase.isOpen());
    return;
    }
  //this->m_DbPath = db;
  //this->LoadStudies();
}

void qCTKDCMTKModel::rootRowsAboutToBeInserted(const QModelIndex& rootParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  QModelIndex index = this->createIndex(rootParent.row(), rootParent.column(), d->RootNode);
  this->beginInsertRows(index, start, end);
}

void qCTKDCMTKModel::rootRowsInserted(const QModelIndex& rootParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  this->endInsertRows();
}

void qCTKDCMTKModel::patientRowsAboutToBeInserted(const QModelIndex& patientParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  QModelIndex index = this->createIndex(patientParent.row(), patientParent.column(), d->PatientNode);
  this->beginInsertRows(index, start, end);
}

void qCTKDCMTKModel::patientRowsInserted(const QModelIndex& patientParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  this->endInsertRows();
}

void qCTKDCMTKModel::studyRowsAboutToBeInserted(const QModelIndex& studyParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  QModelIndex index = this->createIndex(studyParent.row(), studyParent.column(), d->StudyNode);
  this->beginInsertRows(index, start, end);
}

void qCTKDCMTKModel::studyRowsInserted(const QModelIndex& studyParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  this->endInsertRows();
}

void qCTKDCMTKModel::seriesRowsAboutToBeInserted(const QModelIndex& seriesParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  QModelIndex index = this->createIndex(seriesParent.row(), seriesParent.column(), d->SeriesNode);
  this->beginInsertRows(index, start, end);
}

void qCTKDCMTKModel::seriesRowsInserted(const QModelIndex& seriesParent, int start, int end)
{
  QCTK_D(qCTKDCMTKModel);
  this->endInsertRows();
}

void qCTKDCMTKModel::onModelAboutToBeReset()
{
  this->beginResetModel();
}

void qCTKDCMTKModel::onModelReset()
{
  this->endResetModel();
}

void qCTKDCMTKModel::onLayoutAboutToBeChanged()
{
  emit layoutAboutToBeChanged();
}

void qCTKDCMTKModel::onLayoutChanged()
{
  emit layoutChanged();
}
