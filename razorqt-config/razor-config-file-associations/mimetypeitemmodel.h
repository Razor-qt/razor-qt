/* 
 * File:   MimetypeItemModel.h
 * Author: christian
 *
 * Created on 5. maj 2013, 09:18
 */

#ifndef MIMETYPEITEMMODEL_H
#define	MIMETYPEITEMMODEL_H

#include <QAbstractItemModel>

#include "libraries/qtxdg/xdgmime.h"

// Used for MimetypeItemModel::data to return a QVariant wrapping an XdgMimeInfo*
#define MimeInfoRole 32

Q_DECLARE_METATYPE(XdgMimeInfo*)

/*!
 * 
 */
class MimetypeItemModel : public QAbstractItemModel
{
public:
    MimetypeItemModel();
    virtual ~MimetypeItemModel();

    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const {return 1;} 
};

#endif	/* MIMETYPEITEMMODEL_H */

