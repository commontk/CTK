
// qCTKDCMTK includes
#include "qCTKDCMTKDICOMItemTreeModel.h"

//----------------------------------------------------------------------------
class qCTKDCMTKDICOMItemTreeModelPrivate: public qCTKPrivate<qCTKDCMTKDICOMItemTreeModel>
{
public:
  qCTKDCMTKDICOMItemTreeModelPrivate(){}
};

//----------------------------------------------------------------------------
// qCTKDCMTKDICOMItemTreeModelPrivate methods


//----------------------------------------------------------------------------
// qCTKDCMTKDICOMItemTreeModel methods

qCTKDCMTKDICOMItemTreeModel::qCTKDCMTKDICOMItemTreeModel(QObject* _parent):Superclass(_parent)
{
  QCTK_INIT_PRIVATE(qCTKDCMTKDICOMItemTreeModel);
  QCTK_D(qCTKDCMTKDICOMItemTreeModel);

}

//----------------------------------------------------------------------------
qCTKDCMTKDICOMItemTreeModel::~qCTKDCMTKDICOMItemTreeModel()
{
}

//----------------------------------------------------------------------------
QModelIndex qCTKDCMTKDICOMItemTreeModel::index(int row, int column,
                            const QModelIndex &parent) const
{
  return QModelIndex();
}

//----------------------------------------------------------------------------
QModelIndex qCTKDCMTKDICOMItemTreeModel::parent(const QModelIndex &child) const
{
  return QModelIndex();
}

//----------------------------------------------------------------------------
Qt::ItemFlags qCTKDCMTKDICOMItemTreeModel::flags(const QModelIndex &index) const
{
  return 0;
}

//----------------------------------------------------------------------------
QVariant qCTKDCMTKDICOMItemTreeModel::data(const QModelIndex &index, int role) const
{
  return QVariant();
}

//----------------------------------------------------------------------------
QVariant qCTKDCMTKDICOMItemTreeModel::headerData(int section, Qt::Orientation orientation,
                                                 int role) const
{
  return QVariant();
}

//----------------------------------------------------------------------------
int qCTKDCMTKDICOMItemTreeModel::rowCount(const QModelIndex &parent) const
{
  return -1;
}

//----------------------------------------------------------------------------
int qCTKDCMTKDICOMItemTreeModel::columnCount(const QModelIndex &parent) const
{
  return -1;
}
