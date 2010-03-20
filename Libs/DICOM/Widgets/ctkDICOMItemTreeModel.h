#ifndef __ctkDICOMItemTreeModel_h
#define __ctkDICOMItemTreeModel_h

// QT includes 
#include <QAbstractItemModel>
#include <QModelIndex>

// qCTK includes
#include <ctkPimpl.h>

#include "CTKDICOMWidgetsExport.h"

class ctkDICOMItemTreeModelPrivate;

class CTK_DICOM_WIDGETS_EXPORT ctkDICOMItemTreeModel : public QAbstractItemModel
{
public:
  typedef QAbstractItemModel Superclass;
  explicit ctkDICOMItemTreeModel(QObject* parent=0);
  virtual ~ctkDICOMItemTreeModel();

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
  QCTK_DECLARE_PRIVATE(ctkDICOMItemTreeModel);
};

#endif
