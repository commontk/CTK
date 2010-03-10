#ifndef __qCTKDCMTKModel_h
#define __qCTKDCMTKModel_h

// QT includes 
#include <QAbstractItemModel>

// qCTK includes
#include <qCTKPimpl.h>

#include "CTKDICOMCoreExport.h"

class qCTKDCMTKModelPrivate;
class Q_CTK_DICOM_CORE_EXPORT qCTKDCMTKModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  explicit qCTKDCMTKModel(QObject* parent = 0);
  virtual ~qCTKDCMTKModel();

  void setDataBase(const QString& database);

  virtual bool canFetchMore ( const QModelIndex & parent ) const;
  virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
  virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  virtual void fetchMore ( const QModelIndex & parent );
  virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
  virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
  //virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
  virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  virtual QModelIndex parent ( const QModelIndex & index ) const;
  virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  //virtual bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );
protected slots:
  void rootRowsAboutToBeInserted(const QModelIndex& rootParent, int start, int end);
  void rootRowsInserted(const QModelIndex& rootParent, int start, int end);
  void patientRowsAboutToBeInserted(const QModelIndex& patientParent, int start, int end);
  void patientRowsInserted(const QModelIndex& patientParent, int start, int end);
  void studyRowsAboutToBeInserted(const QModelIndex& studyParent, int start, int end);
  void studyRowsInserted(const QModelIndex& studyParent, int start, int end);
  void seriesRowsAboutToBeInserted(const QModelIndex& seriesParent, int start, int end);
  void seriesRowsInserted(const QModelIndex& seriesParent, int start, int end);
  void onModelAboutToBeReset();
  void onModelReset();
  void onLayoutAboutToBeChanged();
  void onLayoutChanged();

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKModel);
};

#endif
