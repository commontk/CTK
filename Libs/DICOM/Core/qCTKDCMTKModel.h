#ifndef __qCTKDCMTKModel_h
#define __qCTKDCMTKModel_h

// QT includes 
#include <QAbstractItemModel>
#include <QSqlDatabase>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKModelPrivate;
class Q_CTK_DICOM_CORE_EXPORT qCTKDCMTKModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit qCTKDCMTKModel(QObject* parent = 0);
  virtual ~qCTKDCMTKModel();

  void setDatabase(const QSqlDatabase& dataBase);

  virtual bool canFetchMore ( const QModelIndex & parent ) const;
  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual void fetchMore ( const QModelIndex & parent );
  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
  virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent ( const QModelIndex & index ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
  virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKModel);
};

#endif
