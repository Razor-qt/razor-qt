/********************************************************************
  Copyright: 2011 Alexander Sokoloff <sokoloff.a@gmail.ru>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License.
  version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*********************************************************************/

#include "addplugindialog.h"
#include "ui_addplugindialog.h"
#include <razorqt/xdgicon.h>
#include "../razorpanel_p.h"
#include <QtGui/QListWidgetItem>
#include <QtGui/QItemDelegate>
#include <QtGui/QTextDocument>
#include <QStyledItemDelegate>
#include <QtGui/QIcon>
#include <QPainter>
#include <QDebug>
#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>

#include <QLineEdit>

#define SEARCH_ROLE  Qt::UserRole
#define INDEX_ROLE   SEARCH_ROLE+1

class HtmlDelegate : public QStyledItemDelegate
{
public:
    HtmlDelegate(QObject *parent = 0): QStyledItemDelegate(parent) {}
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
};


/************************************************

 ************************************************/
void HtmlDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    if (!index.isValid())
        return;

    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    painter->save();

    QTextDocument doc;
    doc.setHtml(options.text);
    QIcon icon = options.icon;

    options.text = "";
    options.icon = QIcon();
    options.widget->style()->drawControl(QStyle::CE_ItemViewItem, &options, painter);

    // Draw icon ................................
    QSize iconSize = icon.actualSize(options.rect.size());
    painter->translate(options.rect.left(), options.rect.top());
    QRect iconRect = QRect(4, 4, iconSize.width(), iconSize.height());

    icon.paint(painter, iconRect);

    // shift text right to make icon visible
    painter->translate(iconRect.right() + 8, 0);
    QRect clip(0, 0, options.rect.width()-iconRect.right()- 10, options.rect.height());


    painter->setClipRect(clip);
    QAbstractTextDocumentLayout::PaintContext ctx;
    // set text color to red for selected item
    if (option.state & QStyle::State_Selected)
    {
        ctx.palette.setColor(QPalette::Text, option.palette.color(QPalette::HighlightedText));
    }
    ctx.clip = clip;
    doc.documentLayout()->draw(painter, ctx);

    painter->restore();
}


/************************************************

 ************************************************/
QSize HtmlDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QStyleOptionViewItemV4 options = option;
    initStyleOption(&options, index);

    QTextDocument doc;
    doc.setHtml(options.text);
    doc.setTextWidth(options.rect.width());
    return QSize(doc.idealWidth(), doc.size().height() + 8);
}


/************************************************

 ************************************************/
AddPluginDialog::AddPluginDialog(RazorPluginInfoList<RazorPanelPluginInfo>* plugins, QWidget *parent):
    QDialog(parent),
    ui(new Ui::AddPluginDialog),
    mPlugins(plugins),
    mTimerId(0)

{
    ui->setupUi(this);
    QListWidget* pluginList = ui->pluginList;

    pluginList->setItemDelegate(new HtmlDelegate(this));
    pluginList->setIconSize(QSize(32, 32));
    //pluginList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(pluginList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(emitPluginSelected()));

    QIcon fallIco = XdgIcon::fromTheme("preferences-plugin", 32);

    for (int i=0; i< mPlugins->length(); ++i)
    {
        const RazorPanelPluginInfo* plugin = mPlugins->at(i);
        qDebug() << plugin->name();
        QListWidgetItem* item = new QListWidgetItem(ui->pluginList);
        item->setText(QString("<b>%1</b><br>\n%2\n").arg(plugin->name(), plugin->comment()));
        item->setIcon(plugin->icon(32, fallIco));
        item->setData(INDEX_ROLE, i);
        item->setData(SEARCH_ROLE, QString("%1 %2 %3 %4").arg(
                        plugin->name(),
                        plugin->comment(),
                        plugin->value("Name").toString(),
                        plugin->value("Comment").toString()
                       )
                     );
    }


    connect(ui->searchEdit, SIGNAL(textEdited(QString)), this, SLOT(searchEditTexChanged(QString)));
    connect(ui->addButton, SIGNAL(clicked(bool)), this, SLOT(emitPluginSelected()));

}


/************************************************

 ************************************************/
AddPluginDialog::~AddPluginDialog()
{
    delete ui;
}


/************************************************

 ************************************************/
void AddPluginDialog::searchEditTexChanged(const QString& text)
{
    if (mTimerId)
        killTimer(mTimerId);

    mTimerId = startTimer(SEARCH_DELAY);
}


/************************************************

 ************************************************/
void AddPluginDialog::timerEvent(QTimerEvent* event)
{
    if (event->timerId() == mTimerId)
    {
        killTimer(mTimerId);
        QListWidget* pluginList = ui->pluginList;
        QString s = ui->searchEdit->text();

        for (int i=0; i < pluginList->count(); ++i)
        {
            QListWidgetItem* item = pluginList->item(i);
            item->setHidden(! item->data(SEARCH_ROLE).toString().contains(s, Qt::CaseInsensitive));
        }
    }
}



/************************************************

 ************************************************/
void AddPluginDialog::emitPluginSelected()
{
    QListWidget* pluginList = ui->pluginList;
    if (pluginList->currentItem())
    {
        RazorPanelPluginInfo* plugin = mPlugins->at(pluginList->currentItem()->data(INDEX_ROLE).toInt());
        emit pluginSelected(plugin);
    }
}
