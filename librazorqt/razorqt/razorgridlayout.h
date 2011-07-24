#ifndef RAZORGRIDLAYOUT_H
#define RAZORGRIDLAYOUT_H

#include <QtGui/QGridLayout>
#include <QtCore/QList>

class QWidget;

class RazorGridLayout : public QGridLayout
{
    Q_OBJECT
public:
    explicit RazorGridLayout(QWidget *parent = 0);
    void addObject(QWidget *widget);
    void setRowsCount(int value);
    void reDo();
    void clearLayout();

private:
    int mRow;
    int mColumn;
    int mRows;
};
#endif // RAZORGRIDLAYOUT_H
