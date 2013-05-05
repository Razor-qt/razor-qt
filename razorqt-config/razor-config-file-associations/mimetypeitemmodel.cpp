/* 
 * File:   MimetypeItemModel.cpp
 * Author: christian
 * 
 * Created on 5. maj 2013, 09:18
 */
#include <QObject>
#include <QDebug>

#include "mimetypeitemmodel.h"
#include "qtxdg/xdgmime.h"

/*
 * Implementation note:
 * 
 * MimetypeItemModel is an implementation of QAbstractItemModel with XdgMimeInfoCache as backing store.
 * 
 * There are 2 levels of items in this model: mediatype ('application', 'audio', 'image' etc.) and 
 * subtype ('application/pdf' which is a child of 'application' or 'image/jpeg' which is a child of 'image'.
 * 
 * A QModelIndex for a mediatype has a zero internal pointer. 
 * 
 * A QModelIndex for a subtype has an internal pointer to the XdgMimeInfo for that subtype - so the QModelIndex for 
 * 'image/jpeg' has an internal pointer that points to the XdgMineInfo for 'image/jpeg' which is held in XdgMimeInfoCache.
 * 
 * QModelIndexes for mediatypes have no parents, and QModelIndexes for subtypes have no children.
 */


MimetypeItemModel::MimetypeItemModel()
{
}

MimetypeItemModel::~MimetypeItemModel()
{
}

QVariant MimetypeItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())   
    {
        return QVariant();
    }
    
    XdgMimeInfo *mimeInfo = 0;
    QString text;

    if (index.parent().isValid())
    {
        mimeInfo = static_cast<XdgMimeInfo*>(index.internalPointer());
        text = mimeInfo->subType();
    }
    else 
    {
        text = XdgMimeInfoCache::mediatypes().value(index.row());
    }

    switch (role)
    {
        case Qt::DisplayRole : return QVariant(text);
        case MimeInfoRole    : return QVariant::fromValue<XdgMimeInfo*>(mimeInfo);
        default              : return QVariant();
    }
}

QModelIndex MimetypeItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column != 0)
    {
        return QModelIndex();
    }
    
    if (parent.isValid())
    {
        if (parent.row() >= XdgMimeInfoCache::mediatypes().size())  
        {
            return QModelIndex();
        }

        QString mediatype = XdgMimeInfoCache::mediatypes().value(parent.row());
       
        if (row >= XdgMimeInfoCache::subtypes(mediatype).size())
        {
            return QModelIndex();
        }
        
        QString subtype = XdgMimeInfoCache::subtypes(mediatype).value(row);
        
        XdgMimeInfo* mimeInfo = XdgMimeInfoCache::xdgMimeInfo(mediatype, subtype);
        return createIndex(row, 0, mimeInfo);
    }
    else 
    {
        if (row >= XdgMimeInfoCache::mediatypes().size())
        {
            return QModelIndex();
        }

        return createIndex(row, 0);
    }
}

QModelIndex MimetypeItemModel::parent(const QModelIndex& index) const
{
    if (index.isValid() && index.internalPointer())
    {
        XdgMimeInfo* mimeInfo = static_cast<XdgMimeInfo*>(index.internalPointer());
    
        int row = XdgMimeInfoCache::mediatypes().indexOf(mimeInfo->mediaType());
        return createIndex(row, 0);
    }
    else 
    {
        return QModelIndex();
    }
}


int MimetypeItemModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    if (parent.parent().isValid())
    {
        return 0;
    }
    else if (parent.isValid())
    {
        QString media = XdgMimeInfoCache::mediatypes().value(parent.row());
        return XdgMimeInfoCache::subtypes(media).size();
    }
    else 
    {
        return XdgMimeInfoCache::mediatypes().size();
    }
}
