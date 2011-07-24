#include "razorgridlayout.h"
#include <QWidget>
#include <QDebug>

RazorGridLayout::RazorGridLayout(QWidget *parent) :
    QGridLayout(parent)
{
    setSpacing(0);
    setMargin(0);
    setContentsMargins(0, 0, 0, 0);

    mRow = 0;
    mColumn = 0;
}

void RazorGridLayout::addObject(QWidget *widget)
{
    addWidget(widget, mRow, mColumn);
    ++mRow;
    if (mRow == mRows)
    {
        mRow = 0;
        ++mColumn;
    }
}

void RazorGridLayout::setRowsCount(int value)
{
    mRows = value;
}

void RazorGridLayout::reDo()
{
   QList<QLayoutItem*> items;
   QLayoutItem *item;

   while ((item = this->takeAt(0)) != 0)
   {
       if (!item->isEmpty())
       {
        items.append(item);
        qDebug() << items.length();
        qDebug() << this->count();
       }
   }

    mRow = 0;
    mColumn = 0;

    foreach (QLayoutItem *it, items)
    {
        addItem(it, mRow, mColumn);
        ++mRow;
        if (mRow == mRows)
        {
            mRow = 0;
            ++mColumn;
        }
    }
}

void RazorGridLayout::clearLayout()
{
    QLayoutItem *item;

    while ((item = this->takeAt(0)) != 0)
    {
        delete item;
    }

    mRow = 0;
    mColumn = 0;
}
