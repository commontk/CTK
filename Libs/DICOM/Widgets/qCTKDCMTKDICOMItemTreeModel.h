#ifndef __qCTKDCMTKDICOMItemTreeModel_h
#define __qCTKDCMTKDICOMItemTreeModel_h

// QT includes 
#include <QAbstractItemModel>
#include <QModelIndex>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class qCTKDCMTKDICOMItemTreeModelPrivate;

class Q_CTK_DICOM_WIDGETS_EXPORT qCTKDCMTKDICOMItemTreeModel : public QAbstractItemModel
{
public:
  typedef QAbstractItemModel Superclass;
  explicit qCTKDCMTKDICOMItemTreeModel(QObject* parent=0);
  virtual ~qCTKDCMTKDICOMItemTreeModel();

  virtual QModelIndex index(int row, int column,
                            const QModelIndex &parent = QModelIndex()) const;
                            
  virtual QModelIndex parent(const QModelIndex &child) const;
    
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

  virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const;

  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
  QCTK_DECLARE_PRIVATE(qCTKDCMTKDICOMItemTreeModel);
};

#endif
